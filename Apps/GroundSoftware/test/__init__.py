import os
import sys

# Insert Project Directory:
# Relative location of the root of the rest of the project
PROJECT_DIR = '../IrisBackendv3'
LOCAL_DIR = os.path.dirname(__file__)
ABS_PROJECT_PATH = os.path.abspath(os.path.join(LOCAL_DIR, PROJECT_DIR))
sys.path.insert(0, ABS_PROJECT_PATH)
