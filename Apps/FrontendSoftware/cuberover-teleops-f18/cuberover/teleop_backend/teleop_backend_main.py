#!/usr/bin/env python3

# Make sure we have the root directory "teleop_backend" in the python path
import inspect
import os
import sys

__THIS_FILE_DIR = os.path.realpath(os.path.dirname(inspect.getfile(inspect.currentframe())))
__TELEOP_BACKEND_DIR = os.path.dirname(__THIS_FILE_DIR)
if __TELEOP_BACKEND_DIR not in sys.path:
    sys.path.insert(0, __TELEOP_BACKEND_DIR)

import argparse
import multiprocessing
import queue

from teleop_backend.network import multiprocess_tcp_client
from teleop_backend.network import message_parsing_state_machine
from teleop_backend.database import multiprocess_db_handler
from teleop_backend.database import mongo_db_collection
from teleop_backend.commands import command
from teleop_backend.utils import signal_utils


def send_messages_and_update_db(application_wide_shutdown_event: multiprocessing.Event,
                                new_command_output_queue: multiprocessing.Queue,
                                command_status_update_queue: multiprocessing.Queue,
                                tcp_client: multiprocess_tcp_client.MultiprocessTcpClient):
    try:
        signal_utils.setup_signal_handler_for_process(shutdown_event=application_wide_shutdown_event)

        while not application_wide_shutdown_event.is_set():
            try:
                new_cmd = new_command_output_queue.get(block=True, timeout=0.5)  # type: command.Command
                lookup_id = new_cmd.lookup_id

                update = None
                try:
                    tcp_client.send(new_cmd.to_encoded_bytes())
                    update = multiprocess_db_handler.StatusUpdate(lookup_id=lookup_id,
                                                                  new_state=mongo_db_collection.MongoDbState.SUCC_SENT)
                except Exception as err:
                    print("[send_messages_and_update_db]: Sending {} failed due to error:"
                          " {}".format(new_cmd.__class__.__name__, err))
                    update = multiprocess_db_handler.StatusUpdate(
                        lookup_id=lookup_id,
                        new_state=mongo_db_collection.MongoDbState.FAIL,
                        err_string="Failed to send due to error: {}".format(err))
                    raise
                finally:
                    if update is not None:
                        print("[send_messages_and_update_db]: Writing status update for lookup id {}:"
                              " {}".format(update.lookup_id, update.get_update_dict()))
                        command_status_update_queue.put(update)

            except queue.Empty:
                pass
    finally:
        application_wide_shutdown_event.set()
        print("[send_messages_and_update_db]: main task exiting")


def main():
    parser = argparse.ArgumentParser(description="Reads commands from database and sends them out")
    parser.add_argument("-a", "--address", type=str, required=True,
                        help="The address to which the client should connect")
    parser.add_argument("-p", "--port", type=int, required=True,
                        help="The port to which the client should connect")
    args = parser.parse_args()

    completed_msg_queue = multiprocessing.Queue()
    msg_parser = message_parsing_state_machine.MessageParsingStateMachine(input_bytes_will_be_big_endian=True)
    msg_parser.register_completed_message_queue(completed_message_queue=completed_msg_queue)

    app_wide_shutdown_event = multiprocessing.Event()
    tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(application_wide_shutdown_event=app_wide_shutdown_event)
    tcp_client.register_data_handler(handler=msg_parser)
    tcp_client.connect(address=args.address, port=args.port)

    new_command_output_queue = multiprocessing.Queue()
    command_status_update_queue = multiprocessing.Queue()
    db_handler = multiprocess_db_handler.MultiprocessDbHandler(app_wide_shutdown_event,
                                                               new_command_output_queue,
                                                               command_status_update_queue)
    db_handler.start()

    send_messages_and_update_db(app_wide_shutdown_event,
                                new_command_output_queue,
                                command_status_update_queue,
                                tcp_client)


if __name__ == "__main__":
    main()
