#!/usr/bin/env python3

# Make sure we have the root directory "teleop_backend" in the python path
import inspect
import os
import sys

__THIS_FILE_DIR = os.path.realpath(os.path.dirname(inspect.getfile(inspect.currentframe())))
__TELEOP_BACKEND_DIR = os.path.dirname(__THIS_FILE_DIR)
if __TELEOP_BACKEND_DIR not in sys.path:
    sys.path.insert(0, __TELEOP_BACKEND_DIR)

import copy
import multiprocessing
import random
import time

from teleop_backend.database import mongo_db_collection
from teleop_backend.utils import signal_utils


def main():
    db = mongo_db_collection.MongoDbCollection()

    left = {'args': {'Angle': 111, 'Images': 4, 'Speed': 11}, 'lookupID': 13, 'name': 'TurnLeft'}
    right = {'args': {'Angle': 13, 'Images': 12, 'Speed': 3}, 'lookupID': 13, 'name': 'TurnRight'}
    forward = {'args': {'Distance': 56, 'Images': 0, 'Speed': 1}, 'lookupID': 13, 'name': 'MoveForward'}
    backward = {'args': {'Distance': 200, 'Images': 60, 'Speed': 99}, 'lookupID': 13, 'name': 'MoveBackward'}
    cmds = [left, right, forward, backward]

    app_wide_shutdown_event = multiprocessing.Event()
    signal_utils.setup_signal_handler_for_process(shutdown_event=app_wide_shutdown_event)
    while not app_wide_shutdown_event.is_set():
        wait_time_until_next = random.random() * 5.0
        cmd_to_send = random.choice(cmds)
        db.write(copy.deepcopy(cmd_to_send))
        print("[{}]: Just wrote new cmd: {}".format(time.time(), cmd_to_send))
        time.sleep(wait_time_until_next)

if __name__ == "__main__":
    main()
