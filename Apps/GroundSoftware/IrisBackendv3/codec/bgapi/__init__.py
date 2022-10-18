import os

# Expose main BGAPI library endpoints:
from bgapi.bglib import BGLib
from bgapi.apiparser import ParsedApi
from bgapi.serialconnector import SerialConnector
from bgapi.socketconnector import SocketConnector

# Expose main BGAPI message types:
from bgapi.bglib import BGMsg, BGCommand, BGResponse, BGEvent

# Expose our custom handlers:
from .custom_listener import BGLibListener

# Also expose API paths:
BGAPI_DIR = os.path.dirname(os.path.realpath(__file__))
BGAPI_WIFI_DEF = os.path.join(BGAPI_DIR, "./bgapi_wifi_1.4.1.xml")
