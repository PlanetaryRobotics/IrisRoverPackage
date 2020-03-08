#!/usr/bin/env python3

# system imports
import multiprocessing
import pathlib
import queue
import sys
import time
import typing

import select
import socket

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
from teleop_backend.msg_format import format_conversion
from teleop_backend.pipeline import pipeline
from teleop_backend.pipeline.consumers import event_consumer
from teleop_backend.pipeline.consumers import command_consumer
from teleop_backend.pipeline.consumers import telemetry_consumer
from teleop_backend.network import message_parsing_state_machine
from teleop_backend.utils import signal_utils

SERVER_SELECT_TIMEOUT = 0.01
RECV_MAX_SIZE = 4096


class FakeServerPipeline:
    def __init__(self):
        # Our queues
        self.__received_db_object_queue = None
        self.__received_serialized_msg_queue = None
        self.__send_queue = None

        # Some f prime stuff
        self.__the_cmd_encoder = None
        self.__response_opcode = None
        self.__command_id_dict = None

        # The event used to shut down all of the processes
        self.__application_wide_shutdown_event = None

        # Our subprocesses
        self.__message_parser_pump_process = None
        self.__server_proc = None

    def build_pipeline(self,
                       address: str,
                       port: int,
                       response_msg_name: str,
                       generated_file_directory_path: pathlib.Path):

        generated_events_path = generated_file_directory_path.joinpath("events")
        generated_telemetry_path = generated_file_directory_path.joinpath("channels")
        generated_commands_path = generated_file_directory_path.joinpath("commands")

        # Create Dictionaries
        event_loader = event_py_loader.EventPyLoader()
        event_id_dict, event_name_dict = event_loader.construct_dicts(str(generated_events_path))

        command_loader = cmd_py_loader.CmdPyLoader()
        self.__command_id_dict, command_name_dict = command_loader.construct_dicts(str(generated_commands_path))

        if response_msg_name not in command_name_dict:
            raise ValueError("Response message name ({}) is not in dictionary of command names (dictionary keys: "
                               "{})".format(response_msg_name, list(command_name_dict.keys())))

        self.__response_opcode = command_name_dict[response_msg_name].get_op_code()

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
        the_command_decoder = cmd_decoder.CmdDecoder(cmd_dict=self.__command_id_dict)
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
            pipeline.MultiprocessingRunner(
                task_name="message_parsing_state_machine_pump",
                application_wide_shutdown_event=self.__application_wide_shutdown_event,
                function_to_repeatedly_run=FakeServerPipeline.transfer_msg_from_parser_queue_to_decoder,
                function_args=[self.__received_serialized_msg_queue, decoder_dict])

        self.__send_queue = multiprocessing.Queue()
        self.__the_cmd_encoder = cmd_encoder.CmdEncoder()

        self.__server_proc = multiprocessing.Process(target=FakeServerPipeline.server_recv_task,
                                                     name="teleop_fake_server_recv_task",
                                                     args=(self.__application_wide_shutdown_event,
                                                           address,
                                                           port,
                                                           SERVER_SELECT_TIMEOUT,
                                                           msg_parser,
                                                           self.__send_queue))

        self.__message_parser_pump_process.start()
        self.__server_proc.start()

    def spin(self):
        # This will block until we exit.
        try:
            signal_utils.setup_signal_handler_for_process(shutdown_event=self.__application_wide_shutdown_event)

            while not self.__application_wide_shutdown_event.is_set():
                self.command_receiver_and_responder()

        finally:
            self.__application_wide_shutdown_event.set()
            print("[{}]: Task exiting".format("command_receiver_and_responder"))

    def command_receiver_and_responder(self):
        try:
            db_format_msg = self.__received_db_object_queue.get(block=True, timeout=0.5)

            if db_format_msg["type"] != "command":
                print("Got a message that is not a command: {}".format(db_format_msg), file=sys.stderr)
                return

            # Print our received message
            print("[{}]: {}".format(time.time(), str(db_format_msg)))

            # Send the response, for now always success response
            response = {
                "type": "command",
                "opcode": self.__response_opcode,
                "args": {
                    "type": db_format_msg["opcode"],
                    "command": db_format_msg["args"]["command"],
                    "errorCode": 0,
                    "additionalData": 0
                }
            }

            self.send_response(response)
        except queue.Empty:
            pass

    # @staticmethod
    # def telemetry_and_event_sender(application_wide_shutdown_event: multiprocessing.Event,
    #                                new_command_output_queue: multiprocessing.Queue,
    #                                command_status_update_queue: multiprocessing.Queue,
    #                                the_telem_encoder: ch_encoder.ChEncoder,
    #                                the_event_encoder: event_encoder.EventEncoder,
    #                                tcp_client: multiprocess_tcp_client.MultiprocessTcpClient):
    #     try:
    #         signal_utils.setup_signal_handler_for_process(shutdown_event=application_wide_shutdown_event)
    #
    #         while not application_wide_shutdown_event.is_set():
    #             # TODO: Actually implement
    #             pass
    #
    #     finally:
    #         application_wide_shutdown_event.set()
    #         print("[{}]: Task exiting")

    def send_response(self, response_in_db_format: dict):
        response_data_obj = \
            format_conversion.convert_command_from_database_to_data_type(response_in_db_format, self.__command_id_dict)
        serialized_cmd_bytes = self.__the_cmd_encoder.encode_api(response_data_obj)
        self.send_message(serialized_cmd_bytes)

    def send_message(self, serialized_cmd_bytes: bytes):
        full_serialized_cmd_bytes = b"A5A5 GUI %s" % serialized_cmd_bytes
        self.__send_queue.put(full_serialized_cmd_bytes)

    @staticmethod
    def transfer_msg_from_parser_queue_to_decoder(received_message_output_queue: multiprocessing.Queue,
                                                  decoder_dict: typing.Dict[
                                                      data_desc_type.DataDescType, decoder.Decoder]):
        # For now, we don't do anything with the header bytes. The msg contents go to the appropriate decoder
        try:
            queue_obj = received_message_output_queue.get(block=True, timeout=0.5)
            #print("queue_obj:", queue_obj)
            msg_type, _, new_msg_content_bytes = queue_obj
            #print("msg_type:", msg_type)
            #print("decoder_dict:", decoder_dict)
            #print("new_msg_content_bytes:", new_msg_content_bytes)
            decoder_dict[msg_type].data_callback(new_msg_content_bytes)
        except queue.Empty:
            pass

    @staticmethod
    def server_recv_task(shutdown_event,
                         address, port,
                         select_timeout,
                         data_handler,
                         send_queue: multiprocessing.Queue):
        sock = None
        try:
            signal_utils.setup_signal_handler_for_process(shutdown_event=shutdown_event)

            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.bind((address, port))
            sock.settimeout(1)
            sock.listen(1)
            while not shutdown_event.is_set():
                try:
                    # Clear any old messages from the queue before accepting a new client connection
                    while not send_queue.empty():
                        send_queue.get_nowait()

                    conn, addr = sock.accept()
                    with conn:
                        print('Connection from', addr)
                        while not shutdown_event.is_set():
                            ready = select.select([conn], [], [conn], select_timeout)
                            if ready[0]:
                                chunk = conn.recv(RECV_MAX_SIZE)
                                if len(chunk) == 0:
                                    print("Peer closed the connection")
                                    break
                                else:
                                    print("Got {} bytes of data".format(len(chunk)))
                                data_handler.new_bytes(chunk)
                            elif ready[2]:
                                break

                            try:
                                msg_bytes_to_send = send_queue.get(block=False)
                                result = conn.sendall(msg_bytes_to_send, 0)

                                if result is None:
                                    print("[server_recv_task]: Send successful")
                                else:
                                    print("[server_recv_task]: Send unsuccessful, result={}".format(result))
                            except queue.Empty:
                                pass

                        print("Disconnected")
                except socket.timeout:
                    pass
        finally:
            if sock:
                sock.close()

            shutdown_event.set()
            print("Server recv task exiting")
