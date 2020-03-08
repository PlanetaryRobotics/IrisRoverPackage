import collections
import multiprocessing
import queue
import sys
import time
import typing

from fprime_gds.common.templates import cmd_template

from teleop_backend.database import mongo_db_collection
from teleop_backend.utils import signal_utils
from teleop_backend.msg_format import format_conversion


class StatusUpdate:
    """An update to the status of a command in the database.

    Attributes:
        lookup_id: The integer lookup ID of the command in the database to be updated.
        cmd_opcode: The integer opcode of the command being updated.
        new_state: The new state of the command in the database, or None if the state shouldn't be updated.
        err_string: The new error string of the command in the database, or None if if the state shouldn't be updated.
    """

    def __init__(self, lookup_id: int, cmd_opcode: int, *,
                 new_state: mongo_db_collection.MongoDbState = None,
                 err_string: str = None):
        """Constructor.

        Arguments:
            Same as the class attributes. See the class attribute descriptions for more details.
        """
        self.lookup_id = lookup_id
        self.cmd_opcode = cmd_opcode
        self.new_state = new_state
        self.err_string = err_string

    def get_update_dict(self) -> dict:
        """Gets the dictionary of key-value pairs to be updated in the command object in the database for this update.

        This update dictionary will consist of a 'stateFp' key and state value if this update changes the state of the
        command, and an 'errorFp' key and string if this update changes the error message of the command.

        Returns:
            The command update dictionary.
        """
        update_dict = {}
        if self.new_state is not None:
            update_dict['stateFp'] = self.new_state.value

        if self.err_string is not None:
            update_dict['errorFp'] = self.err_string

        return update_dict


class DbHandler:
    """Encapsulates writing messages and status updates to the database and retrieving commands from it.

    This class exists primarily for convenience, to hold the queues and other common information associated with these
    database operations. The actual invocation of the methods in the class is done by the DbHandlerProcess class, which
    is defined below this class.

    Commands retrieved from the database are converted into F Prime library command data objects using the
    format_conversion module, then these command data objects are placed onto the new command output queue to be
    further handled by another process running parallel to the DbHandler. However, when changes are made to a command
    in the database (e.g. status updates) the database change stream thinks that a new command has been written to the
    database. To avoid re-processing these commands, a deque of the lookup IDs of all recently updated commands is
    maintained. If a "new" command is retrieved from the dictionary but its lookup ID is already in this deque, the
    command is simply ignored.

    Messages received from the rover (which could be commands, telemetry or events) are received via the received
    database object queue, and are expected to already be in a database format (i.e. a dictionary with the
    appropriate contents for the message type). Telemetry messages, event messages, and command messages other than
    Response commands are all written directly into the database as new database objects with no further processing.
    However, Response commands are parsed and converted into an appropriate StatusUpdate object so that the
    appropriate command in the queue can have its status updated.

    In order to be able to process Response commands as described above, we need to keep track of several things. First
    of all, there is a restriction that two commands with the same opcode cannot be in flight at the same time. Here
    "in flight" means that the command has neither failed to be processed or sent, nor has a Response message been
    received with an update on the result of executing the command. In order to update the status of a command, we need
    a lookup ID to identify the database entry to be updated. However, lookup IDs are not sent to/from the rover, so we
    need to keep track of lookup IDs here. This is accomplished by taking advantage of the restriction that only one
    command per opcode can be in flight at any given time. When a new command is retrieved from the database, we store
    the lookup ID of the command in a map from the command's opcode to its lookup ID. Then, when we receive a Response
    for a particular opcode (which is information that is included in the Response message) then we can find the
    appropriate lookup ID using this map. After a command has received a Response it the mapping for that opcode is
    removed from that dictionary, which then allows a new message with that opcode to be sent.

    The final task handled by this class is inserting command status updates into the database. A command's status may
    be updated if the command database object is malformed or illegal, if another command with the same opcode is
    already in flight, if some kind of error occurs while converting the command into an F Prime command data object
    and subsequently into serialized bytes, if sending the command fails, or if a Response message is received for the
    command. Inserting the status update involves updating the state and/or error message of the command object in the
    database.

    Attributes:
        __cmd_id_dict: A dictionary mapping from command opcode to an F Prime command template for that command.
                       This is required to be able to convert commands in database format into F Prime command data
                       objects.
        __response_cmd_opcode: The integer opcode of the Response command.
        __new_command_output_queue: The "new command output queue" mentioned in the class description. F Prime command
                                    data objects made from new commands in the database are placed onto this queue to be
                                    further handled by another process.
        __received_db_object_queue: The "received database object queue" mentioned in the class description.
                                    Dictionaries of database-formatted messages sent by the rover are placed onto this
                                    queue by another process, and then this class pulls those dictionaries from the
                                    queue and updates the database accordingly based on the type of message.
        __command_status_update_queue: A queue containing StatusUpdate objects created by other processes with an update
                                       to the status of a command. This class pulls those updates and inserts them into
                                       the database.
        __recently_updated_lookup_ids: A deque of the lookup IDs of recently updated commands, used to prevent
                                       reprocessing commands in the database.
        __in_flight_message_dict: A dictionary containing mappings from command opcodes to the full database-format
                                  command dictionary for the command with that opcode that is currently in flight.
        __db_collection: The database wrapper object used to read from and write to the database.
    """

    def __init__(self,
                 cmd_id_dict: typing.Dict[int, cmd_template.CmdTemplate],
                 response_cmd_opcode: int,
                 new_command_output_queue: multiprocessing.Queue,
                 received_db_object_queue: multiprocessing.Queue,
                 command_status_update_queue : multiprocessing.Queue):
        """Constructor.

        Args:
            Same as the similarly named class attributes. See the class attribute descriptions for more details.
        """
        self.__cmd_id_dict = cmd_id_dict
        self.__response_cmd_opcode = response_cmd_opcode
        self.__new_command_output_queue = new_command_output_queue
        self.__received_db_object_queue = received_db_object_queue
        self.__command_status_update_queue = command_status_update_queue
        self.__recently_updated_lookup_ids = collections.deque(maxlen=100)
        self.__in_flight_message_dict = {}  # type: typing.Dict[int, dict]
        self.__db_collection = mongo_db_collection.MongoDbCollection()

    def push_failed_command_status_update(self, lookupId: int, opcode: int, error_msg: str,
                                          *, error_msg_prefix="Failed to send command: "):
        """A convenience method that constructs a StatusUpdate with a FAIL status and pushes it into the database.

        Args:
            lookupId: The lookup ID of the command being updated.
            opcode: The opcode of the command being updated.
            error_msg: The error message contents. The final error message will be error_msg_prefix + error_msg.
            error_msg_prefix: The error message prefix. The final error message will be error_msg_prefix + error_msg.

        Returns:
            None
        """
        status_update = StatusUpdate(lookupId, opcode,
                                     new_state=mongo_db_collection.MongoDbState.FAIL,
                                     err_string="{}{}".format(error_msg_prefix, error_msg))
        print("[push_failed_command_status_update]: Writing status update for lookup id {}:"
              " {}".format(status_update.lookup_id, status_update.get_update_dict()))
        self.push_status_update_to_db(status_update)

    def make_status_update_from_rover_response_command(self, rover_msg_dict: dict) -> typing.Optional[StatusUpdate]:
        """Constructs a status update based on a Response message from the rover.

        If the Response message is malformed or is for an opcode not in the in flight message dictionary then an error
        message will be printed and None will be returned.

        Args:
            rover_msg_dict: The database-format command dictionary for the received Response message.

        Returns:
            A StatusUpdate based on the contents of the Response, or None if an error of some kind occurred.
        """
        # See if we can get the opcode of the command being responded to
        rover_msg_args = rover_msg_dict["args"]
        if "type" not in rover_msg_args:
            print("[DbHandler]: Got a response command from the rover, but its \"args\" dictionary is "
                  "missing an expected field: {}. Full rover message dictionary: {}".format("type", rover_msg_dict),
                  file=sys.stderr)
            return None

        type_arg = rover_msg_args["type"]
        if type_arg not in self.__in_flight_message_dict:
            print("[DbHandler]: Got a response command from the rover whose type argument value ({}) is "
                  "not the opcode of a command that has not yet received a response. The opcodes that are waiting for "
                  "response are {}".format(type_arg, list(self.__in_flight_message_dict.keys())),
                  file=sys.stderr)
            return None

        # For this dict to have made it into the in_flight_message_dict it must have been validated, i.e. we can
        # assume it has a "lookupId" key-value pair.
        dict_of_msg_being_responded_to = self.__in_flight_message_dict[type_arg]
        lookup_id_being_responded_to = dict_of_msg_being_responded_to["lookupID"]

        # Verify that this response command has all of the args we expect
        expected_response_args = ["command", "errorCode", "additionalData"]
        for arg_name in expected_response_args:
            if arg_name not in rover_msg_args:
                print("[DbHandler]: Got a response command from the rover, but its \"args\" dictionary is "
                      "missing an expected argument: {}. Full rover message dictionary: {}".format(arg_name,
                                                                                                   rover_msg_dict),
                      file=sys.stderr)
                self.push_failed_command_status_update(lookup_id_being_responded_to, type_arg,
                                                       "The response message from the rover for this command is "
                                                       "missing an expected argument: {}".format(arg_name))
                return None

        # Construct and push our status update
        command_arg = rover_msg_args["command"]
        error_code_arg = rover_msg_args["errorCode"]
        additional_data_arg = rover_msg_args["additionalData"]

        # A non-zero error code means the command failed to execute
        if error_code_arg == 0:
            status_state = mongo_db_collection.MongoDbState.SUCC_EXEC
            status_msg = "Success"
        else:
            status_state = mongo_db_collection.MongoDbState.FAIL
            status_msg = "Command failed: type={}, command={}, errorCode={}, " \
                         "additionalData={}".format(type_arg, command_arg, error_code_arg, additional_data_arg)

        return StatusUpdate(lookup_id_being_responded_to, type_arg,
                            new_state=status_state,
                            err_string=status_msg)

    def process_new_messages_from_rover(self) -> None:
        """Processes all new messages in the queue of messages received from the rover.

        Steps:
          1) Determine if event, telemetry, or command
          2) If event or telemetry insert into database
          3) If command, ensure it is Response type. (if not, write it into the database as a new entry)
          4) Find the lookup ID for the opcode of the command being responded to
          5) Create StatusUpdate for this response
          6) Push StatusUpdate into database.

        Unexpected or malformed messages will result in an error message being printed, but no exception will be raised.

        Returns:
            None
        """
        try:
            # We'll break out of this while loop when the queue is empty
            while True:
                # Since these rover_msg_dict's will have been originally generated by the functions in the
                # format_conversion module, it should be safe to assume that they will properly have the "type",
                # "name", "opcode", and "args" fields.
                rover_msg_dict = self.__received_db_object_queue.get_nowait()  # type: dict

                print("[DbHandler]: Got new message from rover:", rover_msg_dict)

                msg_type = rover_msg_dict["type"]
                if msg_type == "event" or msg_type == "telemetry":
                    # For event and telemetry, all we need to do is write the message to the database
                    self.__db_collection.write(rover_msg_dict)
                    continue

                elif msg_type != "command":
                    print("[DbHandler]: Got a rover message dictionary that has an unexpected message "
                          "type: {}. Full rover message dictionary: {}".format(msg_type, rover_msg_dict),
                          file=sys.stderr)
                    continue

                # We know msg_type == "command" here
                cmd_opcode = rover_msg_dict["opcode"]
                if cmd_opcode == self.__response_cmd_opcode:
                    status_update = self.make_status_update_from_rover_response_command(rover_msg_dict)

                    if status_update:
                        print("[DbHandler]: Got new status update from rover for lookup id {}:"
                              " {}".format(status_update.lookup_id, status_update.get_update_dict()))
                        self.push_status_update_to_db(status_update)
                else:
                    # For a non-response command, just write it directly into the database
                    self.__db_collection.write(rover_msg_dict)
        except queue.Empty:
            pass


    def process_new_commands_from_db(self) -> None:
        """Gets all new commands from the database and processes them appropriately.

        A properly formatted and actually new command will be converted to an F Prime command data object and placed
        into the new command output queue. Then, the command will be recorded in the in flight message dictionary.

        If the command is malformed an error message will be printed and, if possible, a status update will be written
        to the database. If the command is malformed in a way that prevents this (i.e. no lookup ID or no opcode) then
        no such status update will occur.

        If a command has a lookup ID already in the recently updated lookup ID deque, then it will be ignored (though a
        message will printed).

        If converting the database object to an F Prime command data object fails, an error will be logged and a
        status update will occur.

        Returns:
            None
        """
        # Handle any new commands in the database
        new_commands = self.__db_collection.changes.get_all_new()
        for new_cmd_dict in new_commands:
            if "lookupID" not in new_cmd_dict:
                print("[DbHandler]: Got a new command object from the database that does not contain a "
                      "lookupID field. The new command object is:", new_cmd_dict, file=sys.stderr)
                continue
            elif "opcode" not in new_cmd_dict:
                print("[DbHandler]: Got a new command object from the database that does not contain an "
                      "opcode field. The new command object is:", new_cmd_dict, file=sys.stderr)
                continue

            lookup_id = new_cmd_dict["lookupID"]
            if lookup_id in self.__recently_updated_lookup_ids:
                print("[DbHandler]: Ignoring new command in change stream because its lookup ID "
                      "was in the recently updated lookup ID buffer (lookup ID = {})".format(lookup_id))
                continue

            opcode = new_cmd_dict["opcode"]
            if opcode in self.__in_flight_message_dict:
                opcode = new_cmd_dict["opcode"]
                print("[DbHandler]: Got a new command object from the database, but a command with the same"
                      " opcode ({}) is already in flight and waiting for a response".format(opcode),
                      file=sys.stderr)
                self.push_failed_command_status_update(lookup_id, opcode,
                                                       "A command with this opcode ({}) is already in flight and "
                                                       "waiting for a response, so this command will not be "
                                                       "sent".format(opcode))
            else:
                print("[DbHandler]: Got new command from database:", new_cmd_dict)

                try:
                    # Convert the database object to a data object
                    new_cmd_data_obj = format_conversion.convert_command_from_database_to_data_type(new_cmd_dict,
                                                                                                    self.__cmd_id_dict)

                    # Push out the data object on the output queue
                    self.__new_command_output_queue.put((lookup_id, new_cmd_data_obj))

                    self.__in_flight_message_dict[opcode] = new_cmd_dict
                except format_conversion.DatabaseObjectFormatError as e:
                    self.push_failed_command_status_update(lookup_id, opcode, str(e))

    def push_status_update_to_db(self, status_update: StatusUpdate):
        """A convenience function that inserts a status update into the database and updates status dictionaries.

        "updates status dictionaries" specifically means that the lookup ID of this status update will be inserted in
        the recently updated lookup ID deque if it is not already in there, and if the status of this status update
        indicates that the command failed or that it has successfully executed then the entry in the in flight message
        dictionary for this command is removed.

        Args:
            status_update: The command status update to be pushed into the database.

        Returns:
            None
        """
        self.__db_collection.update(status_update.lookup_id, status_update.get_update_dict())
        if status_update.lookup_id not in self.__recently_updated_lookup_ids:
            self.__recently_updated_lookup_ids.append(status_update.lookup_id)
            print("[push_status_update_to_db]: Adding {} to recently looked up ids".format(status_update.lookup_id))

        if (status_update.new_state == mongo_db_collection.MongoDbState.SUCC_EXEC or
                status_update.new_state == mongo_db_collection.MongoDbState.FAIL):
            self.__in_flight_message_dict.pop(status_update.cmd_opcode, None)

    def push_any_available_status_updates_to_db(self):
        """Pulls any available StatusUpdates from the command status update queue then inserts them into the database.

        The StatusUpdates are pushed in the database using push_status_update_to_db, so updates to the recently updated
        lookup ID deque and the in flight message dictionary will also occur, if appropriate.

        Returns:
            None
        """
        try:
            # We'll break out of this while loop when the queue is empty
            while True:
                status_update = self.__command_status_update_queue.get_nowait()  # type: StatusUpdate
                print("[MultiprocessDbHandler]: Got new status update from queue")
                self.push_status_update_to_db(status_update)
        except queue.Empty:
            pass


class DbHandlerProcess:
    """A wrapper class for a multiprocessing.Process that executes the behavior of the DbHandler class.

    Attributes:
        __proc: The multiprocessing.Process that this class wraps.
        __cmd_id_dict: Same as DbHandler.__cmd_id_dict, see DbHandler class documentation.
        __response_cmd_opcode: Same as DbHandler.__response_cmd_opcode, see DbHandler class documentation.
        __application_wide_shutdown_event: The multiprocessing.Event used to signal the shutdown of all processes
                                           running as part of the teleop_backend.
        __this_handler_shutdown_event: The multiprocessing.Event used to signal the shutdown of only __proc.
        __new_command_output_queue: Same as DbHandler.__new_command_output_queue, see DbHandler class documentation.
        __received_db_object_queue: Same as DbHandler.__received_db_object_queue, see DbHandler class documentation.
        command_status_update_queue: Same as DbHandler.command_status_update_queue, see DbHandler class documentation.
        __join_timeout_until_terminate: The number of seconds to wait after joining __proc during shutdown before
                                        calling __proc.terminate().
    """


    def __init__(self,
                 cmd_id_dict: typing.Dict[int, cmd_template.CmdTemplate],
                 response_cmd_opcode: int,
                 application_wide_shutdown_event: multiprocessing.Event,
                 new_command_output_queue: multiprocessing.Queue,
                 incoming_message_queue: multiprocessing.Queue,
                 command_status_update_queue: multiprocessing.Queue,
                 join_timeout_until_terminate=2):
        """Constructor.

        Args:
            Same as similarly named class attributes. Read class attribute documentation for more information.
        """
        self.__proc = None
        self.__cmd_id_dict = cmd_id_dict
        self.__response_cmd_opcode = response_cmd_opcode
        self.__application_wide_shutdown_event = application_wide_shutdown_event
        self.__this_handler_shutdown_event = multiprocessing.Event()
        self.__new_command_output_queue = new_command_output_queue
        self.__received_db_object_queue = incoming_message_queue
        self.__command_status_update_queue = command_status_update_queue
        self.__join_timeout_until_terminate = join_timeout_until_terminate

    def start(self):
        """Constructs the multiprocessing.Process for the db_interaction_task and then starts it.

        Returns:
            None
        """
        self.__proc = multiprocessing.Process(target=DbHandlerProcess.db_interaction_task,
                                              name="DbHandlerProcess_db_interaction_task",
                                              args=(self.__application_wide_shutdown_event,
                                                    self.__this_handler_shutdown_event,
                                                    self.__cmd_id_dict,
                                                    self.__response_cmd_opcode,
                                                    self.__new_command_output_queue,
                                                    self.__received_db_object_queue,
                                                    self.__command_status_update_queue))
        self.__proc.start()

    def shutdown(self):
        """Shuts down the internal process and waits for it to end, terminating it if the wait times out.

        Returns:
            None
        """
        self.__this_handler_shutdown_event.set()

        # We give one second after the select will time out for the recv thread to
        try:
            self.__proc.join(timeout=self.__join_timeout_until_terminate)
        except multiprocessing.TimeoutError:
            self.__proc.terminate()

    @staticmethod
    def db_interaction_task(app_wide_shutdown_event: multiprocessing.Event,
                            this_handler_shutdown_event: multiprocessing.Event,
                            cmd_id_dict: typing.Dict[int, cmd_template.CmdTemplate],
                            response_cmd_opcode: int,
                            new_command_output_queue: multiprocessing.Queue,
                            incoming_message_queue: multiprocessing.Queue,
                            command_status_update_queue : multiprocessing.Queue):
        """The process task method that invokes all repeated DbHandler behavior.

        This method processes all available messages from the rover, then process all command status updates, then
        processes all new commands in the database, and does this in a loop that runs at 5 Hz (or at the very least
        sleeps for 0.2 seconds between loops).

        Args:
            Same as similarly named class attributes. Read class attribute documentation for more information.
        """

        try:
            signal_utils.setup_signal_handler_for_process(shutdown_event=this_handler_shutdown_event)
            db_handler = DbHandler(cmd_id_dict,
                                   response_cmd_opcode,
                                   new_command_output_queue,
                                   incoming_message_queue,
                                   command_status_update_queue)

            while not app_wide_shutdown_event.is_set() and not this_handler_shutdown_event.is_set():
                # Handle any messages from the rover
                db_handler.process_new_messages_from_rover()

                # Handle any internal command status updates if we have them (we'll have these when we fail or succeed
                # to send a command to the rover)
                db_handler.push_any_available_status_updates_to_db()

                # Next, handle any new commands in the database
                db_handler.process_new_commands_from_db()

                # Finally, sleep for 0.2 seconds so that this loop runs at 5 Hz.
                time.sleep(0.2)

        finally:
            app_wide_shutdown_event.set()
            print("[MultiprocessDbHandler]: db interaction task exiting")
