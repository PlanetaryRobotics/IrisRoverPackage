# system imports
import multiprocessing
import pathlib
import queue
import sys
import time
import typing

# fprime imports
from fprime_gds.common.loaders import ch_py_loader
from fprime_gds.common.loaders import event_py_loader
from fprime_gds.common.loaders import cmd_py_loader

from fprime_gds.common.decoders import decoder
from fprime_gds.common.decoders import ch_decoder
from fprime_gds.common.decoders import event_decoder
from fprime_gds.common.encoders import cmd_encoder

from fprime_gds.common.utils import data_desc_type

# teleop_backend imports
from teleop_backend.fprime_gds_additions.common.decoders import cmd_decoder
from teleop_backend.pipeline.consumers import event_consumer
from teleop_backend.pipeline.consumers import command_consumer
from teleop_backend.pipeline.consumers import telemetry_consumer
from teleop_backend.network import multiprocess_tcp_client
from teleop_backend.network import message_parsing_state_machine
from teleop_backend.database import multiprocess_db_handler
from teleop_backend.database import mongo_db_collection
from teleop_backend.utils import signal_utils


class Pipeline:
    """Manages the overall data pipeline, in both directions, of the teleop backend application.

    This class is responsible for constructing all of the objects that are part of the pipeline as well as the queues
    that connect them, then performing all necessary steps to connect the steps of the pipeline together. Finally, this
    it also handles creating and then starting the processes that make the teleop backend function.

    Please see teleop_backend_data_flow.svg in the teleop_backend directory (or, if you have an editor that renders
    Markdown or can view it on GitHub, you can also look at the README.md file in the same location) to see how all of
    the components of the backend connect.

    Attributes:
        __received_db_object_queue: The queue that has database-format message dictionaries put onto it by the command,
                                    event, and telemetry consumers. These dictionaries are then pulled out of the queue
                                    by the database handler process.
        __received_serialized_msg_queue: The queue into which the message parsing state machine places messages parsed
                                         out of the stream of bytes from "Houston". This queue is consumed by the
                                         "message parser pump" process.
        __new_command_output_queue: The queue that has F Prime command data objects placed onto it by the database
                                    handler processes. These data objects represent commands placed into the
                                    database by the frontend. This queue is consumed by the "command from database
                                    sender" process.
        __command_status_update_queue: The queue into which StatusUpdate objects are placed when a commands status is
                                       updated by the "command from database sender process". Specifically, this process
                                       will determine if serializing and sending the command was successful, and will
                                       update the command's status based on how those actions went.
        __application_wide_shutdown_event: The multiprocessing.Event used to signal the shutdown of all processes that
                                           are part of the pipeline.
        __tcp_client: The TCP client, which contains a process that receives a stream of data from "Houston" and feeds
                      the message parsing state machine. This object additionally provides a function to send data to
                      "Houston", and this function is used by the "command from database sender" process.
        __db_handler: The database handler process.
        __command_from_database_sender_process: The "command from database sender" process, which consumes data objects
                                                from the __new_command_output_queue, serializes them, then sends them
                                                to "Houston". This process will also put a command status update into
                                                the __command_status_update_queue with the result of serializing and
                                                sending the command.
         __message_parser_pump_process: The "message parser pump" process, which consumes segmented serialized messages
                                        from the __received_serialized_msg_queue and feeds the F Prime decoders with
                                        message data.
    """

    def __init__(self):
        """Constructor."""
        # Our queues
        self.__received_db_object_queue = None
        self.__received_serialized_msg_queue = None
        self.__new_command_output_queue = None
        self.__command_status_update_queue = None

        # The event used to shut down all of the processes
        self.__application_wide_shutdown_event = None

        # Our subprocesses
        self.__tcp_client = None
        self.__db_handler = None
        self.__command_from_database_sender_process = None
        self.__message_parser_pump_process = None

    def build_pipeline(self,
                       server_address: str,
                       server_port: int,
                       response_msg_name: str,
                       generated_file_directory_path: pathlib.Path):
        """Constructs all pipeline objects and queues and connects them, then starts all pipeline processes.

        For more details about the pipeline see the class description.

        Args:
            server_address: The IP address of the "Houston" server.
            server_port: The port number of the "Houston" server.
            response_msg_name: The name of the response message.
            generated_file_directory_path: The path of the root directory containing generated python dictionary files.
                                           This directory should contain directories called "channels", "commands", and
                                           "events".

        Returns:
            None
        """

        generated_events_path = generated_file_directory_path.joinpath("events")
        generated_telemetry_path = generated_file_directory_path.joinpath("channels")
        generated_commands_path = generated_file_directory_path.joinpath("commands")

        # Create Dictionaries
        event_loader = event_py_loader.EventPyLoader()
        event_id_dict, event_name_dict = event_loader.construct_dicts(str(generated_events_path))

        command_loader = cmd_py_loader.CmdPyLoader()
        command_id_dict, command_name_dict = command_loader.construct_dicts(str(generated_commands_path))

        if response_msg_name not in command_name_dict:
             raise ValueError("Response message name ({}) is not in dictionary of command names (dictionary keys: "
                              "{})".format(response_msg_name, list(command_name_dict.keys())))

        response_opcode = command_name_dict[response_msg_name].get_op_code()

        telemetry_loader = ch_py_loader.ChPyLoader()
        telemetry_id_dict, telemetry_name_dict = \
            telemetry_loader.construct_dicts(str(generated_telemetry_path))

        # This is the queue of message received from the rover, as tuples of
        # (message type (DataDescType), message header (bytes), and message contents (bytes))
        # The message header/contents is such that the contents are ready for input to the event, command, or telemetry
        # decoders.
        self.__received_serialized_msg_queue = multiprocessing.Queue()
        msg_parser = message_parsing_state_machine.MessageParsingStateMachine()
        msg_parser.register_completed_message_queue(completed_message_queue=self.__received_serialized_msg_queue)

        # This is the queue of messages received from the rover, as database-formatted objects. This is the input to the
        # multiprocess_db_handler, and is the output of message consumers. The message consumers are given the
        # decoded messages by the various decoders, and the decoders are given serialized message contents by the
        # message parsing state machine pump. The message parsing state machine pump is a multiprocessing process that
        # pulls messages from the output queue of the message parsing state machine. The message parsing state machine
        # is fed directly with data received from the network.
        self.__received_db_object_queue = multiprocessing.Queue()

        the_event_consumer = event_consumer.EventConsumer(queue=self.__received_db_object_queue)
        the_command_consumer = command_consumer.CommandConsumer(queue=self.__received_db_object_queue)
        the_telemetry_consumer = telemetry_consumer.TelemetryConsumer(queue=self.__received_db_object_queue)

        the_event_decoder = event_decoder.EventDecoder(event_dict=event_id_dict)
        the_command_decoder = cmd_decoder.CmdDecoder(cmd_dict=command_id_dict)
        the_telemetry_decoder = ch_decoder.ChDecoder(ch_dict=telemetry_id_dict)

        # We need to register the consumers to each of their respective decoders, which will make the decoders pass the
        # decoded message data object to the consumers. The consumers will then convert it to a database-formatted
        # object and then put that db-format object into the input queue to the multiprocess_db_handler
        # (received_db_object_queue)
        the_event_decoder.register(consumer_obj=the_event_consumer)
        the_command_decoder.register(consumer_obj=the_command_consumer)
        the_telemetry_decoder.register(consumer_obj=the_telemetry_consumer)

        # This is a map from the data description type (DataDescType) of the different kinds of messages to the
        # decoder object that is appropriate for decoding that kind of message. This dictionary is used by the message
        # parsing state machine pump to select the correct decoder to which serialized message contents should be
        # passed.
        decoder_dict = {
            # This isn't an error; annoyingly the DataDescType for events is called "LOG"
            data_desc_type.DataDescType["FW_PACKET_LOG"]: the_event_decoder,
            data_desc_type.DataDescType["FW_PACKET_COMMAND"]: the_command_decoder,
            data_desc_type.DataDescType["FW_PACKET_TELEM"]: the_telemetry_decoder
        }

        self.__application_wide_shutdown_event = multiprocessing.Event()

        self.__message_parser_pump_process = \
            MultiprocessingRunner(task_name="message_parsing_state_machine_pump",
                                  application_wide_shutdown_event=self.__application_wide_shutdown_event,
                                  function_to_repeatedly_run=Pipeline.transfer_msg_from_parser_queue_to_decoder,
                                  function_args=[self.__received_serialized_msg_queue, decoder_dict])

        tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(
            application_wide_shutdown_event=self.__application_wide_shutdown_event)
        tcp_client.register_data_handler(handler=msg_parser)

        self.__new_command_output_queue = multiprocessing.Queue()
        self.__command_status_update_queue = multiprocessing.Queue()

        db_handler_process = multiprocess_db_handler.DbHandlerProcess(command_id_dict,
                                                                      response_opcode,
                                                                      self.__application_wide_shutdown_event,
                                                                      self.__new_command_output_queue,
                                                                      self.__received_db_object_queue,
                                                                      self.__command_status_update_queue)

        the_cmd_encoder = cmd_encoder.CmdEncoder()

        self.__command_from_database_sender_process = \
            MultiprocessingRunner(task_name="command_from_database_sender",
                                  application_wide_shutdown_event=self.__application_wide_shutdown_event,
                                  function_to_repeatedly_run=Pipeline.send_cmd_from_db,
                                  function_args=[self.__new_command_output_queue,
                                                 self.__command_status_update_queue,
                                                 the_cmd_encoder,
                                                 tcp_client])

        try:
            while not self.__application_wide_shutdown_event.is_set():
                try:
                    tcp_client.connect(address=server_address, port=server_port)
                    break
                except Exception as e:
                    print("Failed to connect to server due to {}, "
                          "will wait 5 seconds then retry connection".format(e), file=sys.stderr)
                    time.sleep(5)
        except KeyboardInterrupt:
            print("Building of pipeline exiting because of Ctrl-C")
            self.__application_wide_shutdown_event.set()
            return

        db_handler_process.start()
        self.__command_from_database_sender_process.start()
        self.__message_parser_pump_process.start()

    def spin(self):
        """Spin until the pipeline terminates.

        It is expected that the launching process (i.e. main()) will call this after starting the rest of the pipeline.

        Returns:
            None
        """
        # This will block until we exit.
        while not self.__application_wide_shutdown_event.is_set():
            time.sleep(0.25)

    @staticmethod
    def send_cmd_from_db(new_command_output_queue: multiprocessing.Queue,
                         command_status_update_queue: multiprocessing.Queue,
                         the_cmd_encoder: cmd_encoder.CmdEncoder,
                         tcp_client: multiprocess_tcp_client.MultiprocessTcpClient):
        """The function of a process that retrieves converted commands from the database and sends them.

        More specifically, this is a function set up to be used with MultiprocessingRunner. Each invocation of this
        function attempts to retrieve one command from the new command output queue (which contains tuples of the
        lookup ID and the F Prime command data object for a command retrieved from the database), serialize it,
        prepend the header and destination string, then send it to the rover via the TCP client.

        In addition to the above, this task will send a StatusUpdate back to the database via the command status update
        queue. This StatusUpdate will be sent even if serializing or sending the command fails (though in these cases
        the contents of the update will obviously be different from that when these things succeed).

        If there are no new commands from the database on the queue, this task will simply time out after half of a
        second and return
        """
        try:
            lookup_id, cmd_from_db_data_object = new_command_output_queue.get(block=True, timeout=0.5)
            opcode = cmd_from_db_data_object.get_template().get_op_code()

            update = None
            try:
                serialized_cmd_bytes = the_cmd_encoder.encode_api(cmd_from_db_data_object)
                full_serialized_cmd_bytes = b"A5A5 FSW %s" % serialized_cmd_bytes
                tcp_client.send(full_serialized_cmd_bytes)

                update = multiprocess_db_handler.StatusUpdate(lookup_id=lookup_id,
                                                              cmd_opcode=opcode,
                                                              new_state=mongo_db_collection.MongoDbState.SUCC_SENT)
            except Exception as err:
                print("[send_cmd_from_db]: Sending {} failed due to error:"
                      " {}".format(cmd_from_db_data_object.get_name(), err))
                update = multiprocess_db_handler.StatusUpdate(
                    lookup_id=lookup_id,
                    cmd_opcode=opcode,
                    new_state=mongo_db_collection.MongoDbState.FAIL,
                    err_string="Failed to send due to error: {}".format(err))
                raise
            finally:
                if update is not None:
                    print("[send_cmd_from_db]: Writing status update for lookup id {}:"
                          " {}".format(update.lookup_id, update.get_update_dict()))
                    command_status_update_queue.put(update)
        except queue.Empty:
            pass

    @staticmethod
    def transfer_msg_from_parser_queue_to_decoder(received_serialized_message_queue: multiprocessing.Queue,
                                                  decoder_dict: typing.Dict[
                                                      data_desc_type.DataDescType, decoder.Decoder]):
        """The function of a process that retrieves commands from the message parser and forwards them to the decoders.

        More specifically, this is a function set up to be used with MultiprocessingRunner. Each invocation of this
        function attempts to retrieve one command from the output queue of the message parsing state machine, and then
        pass the serialized contents of that message to the appropriate decoder (where "appropriate" is determined by
        the type of the message retrieved from the queue).

        Args:
            received_serialized_message_queue: The output queue of the message parsing state machine. The values in this
                                               queue will be tuples of message type (as a data_desc_type.DataDescType),
                                               message header bytes, and message contents bytes.
            decoder_dict: A dictionary mapping from message types to the F Prime Decoder for that type of message.

        Returns:
            None
        """
        # For now, we don't do anything with the header bytes. The msg contents go to the appropriate decoder
        try:
            msg_type, _, new_msg_content_bytes = received_serialized_message_queue.get(block=True, timeout=0.5)
            decoder_dict[msg_type].data_callback(new_msg_content_bytes)
        except queue.Empty:
            pass


class MultiprocessingRunner:
    """A wrapper class that manages a generic multiprocessing.Process that can repeatedly call a particular function.

    Specifically, the task function executed by the managed Process will repeatedly call a given function with given
    arguments and keyword arguments until the application wide shutdown event is signalled.

    Attributes:
        __proc: The multiprocessing.Process managed by this object.
    """

    def __init__(self, *,
                 task_name: str,
                 application_wide_shutdown_event: multiprocessing.Event,
                 function_to_repeatedly_run: typing.Callable,
                 function_args: list = None,
                 function_kwargs: dict = None):
        """Constructor.

        Args:
            task_name: A name for the task of this runner.
            application_wide_shutdown_event: The event that is used to signal the shutdown of all Processes in this
                                             application.
            function_to_repeatedly_run: A callable that will be called repeatedly by the task of the managed Process.
            function_args: The arguments that will be passed to function_to_repeatedly_run on each invocation.
            function_kwargs: The keyword arguments that will be passed to function_to_repeatedly_run on each invocation.
        """
        function_args = function_args if function_args is not None else []
        function_kwargs = function_kwargs if function_kwargs is not None else {}
        self.__proc = multiprocessing.Process(target=MultiprocessingRunner.runner_task,
                                              name="{}_runner".format(task_name),
                                              args=(task_name,
                                                    application_wide_shutdown_event,
                                                    function_to_repeatedly_run,
                                                    function_args,
                                                    function_kwargs))

    def start(self):
        """Starts the managed Process.

        Returns:
            None
        """
        self.__proc.start()

    @staticmethod
    def runner_task(task_name: str,
                    application_wide_shutdown_event: multiprocessing.Event,
                    function_to_repeatedly_run: typing.Callable,
                    function_args: list,
                    function_kwargs: dict):
        """The function performed by the Process managed by this runner.

        This function will repeatedly call the given function until the application wide shutdown event is signalled.

        Args:
            Same as constructor arguments with the same names, see constructor documentation for more details.
        """
        try:
            signal_utils.setup_signal_handler_for_process(shutdown_event=application_wide_shutdown_event)

            while not application_wide_shutdown_event.is_set():
                function_to_repeatedly_run(*function_args, **function_kwargs)

        finally:
            application_wide_shutdown_event.set()
            print("[{}]: Task exiting".format(task_name))
