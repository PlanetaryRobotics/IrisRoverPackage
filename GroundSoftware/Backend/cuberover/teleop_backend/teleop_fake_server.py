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
import time

import select
import socket

from teleop_backend.network import message_parsing_state_machine
from teleop_backend.utils import signal_utils

SERVER_SELECT_TIMEOUT = 0.5
RECV_MAX_SIZE = 4096


def server_recv_task(shutdown_event,
                     address, port,
                     select_timeout,
                     data_handler):
    sock = None
    try:
        signal_utils.setup_signal_handler_for_process(shutdown_event=shutdown_event)

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind((address, port))
        sock.settimeout(1)
        sock.listen(1)
        while not shutdown_event.is_set():
            try:
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
                    print("Disconnected")
            except socket.timeout:
                pass
    finally:
        if sock:
            sock.close()

        shutdown_event.set()
        print("Server recv task exiting")


def main():
    parser = argparse.ArgumentParser(description="Acts as a fake receiver and prints the commands it receives")
    parser.add_argument("-a", "--address", type=str, required=True,
                        help="The address to which the client should connect")
    parser.add_argument("-p", "--port", type=int, required=True,
                        help="The port to which the client should connect")
    args = parser.parse_args()

    completed_msg_queue = multiprocessing.Queue()
    msg_parser = message_parsing_state_machine.MessageParsingStateMachine(input_bytes_will_be_big_endian=True)
    msg_parser.register_completed_message_queue(completed_message_queue=completed_msg_queue)

    app_wide_shutdown_event = multiprocessing.Event()
    signal_utils.setup_signal_handler_for_process(shutdown_event=app_wide_shutdown_event)

    server_proc = multiprocessing.Process(target=server_recv_task,
                                          name="teleop_fake_server_recv_task",
                                          args=(app_wide_shutdown_event,
                                                args.address,
                                                args.port,
                                                SERVER_SELECT_TIMEOUT,
                                                msg_parser))

    server_proc.start()

    signal_utils.setup_signal_handler_for_process(shutdown_event=app_wide_shutdown_event)
    try:
        while not app_wide_shutdown_event.is_set():
            try:
                received_cmd = completed_msg_queue.get(block=True, timeout=0.5)
                print("[{}]: {}".format(time.time(), str(received_cmd)))
            except queue.Empty:
                pass
    finally:
        app_wide_shutdown_event.set()
        print("Server main task exiting")


if __name__ == "__main__":
    main()
