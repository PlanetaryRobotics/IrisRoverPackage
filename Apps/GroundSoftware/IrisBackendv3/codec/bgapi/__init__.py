import os

from typing import Final

# Expose main BGAPI library endpoints:
from bgapi.bglib import BGLib
from bgapi.apiparser import ParsedApi
from bgapi.serialconnector import SerialConnector
from bgapi.socketconnector import SocketConnector

# Expose main BGAPI message types:
from bgapi.bglib import BGMsg, BGCommand, BGResponse, BGEvent

# Expose Serializer / Deserializer:
from bgapi import serdeser


# Expose our custom handlers:
from .custom_listener import BGLibListener

# Also expose API paths:
BGAPI_DIR: Final = os.path.dirname(os.path.realpath(__file__))
BGAPI_WIFI_DEF: Final = os.path.join(BGAPI_DIR, "./bgapi_wifi_1.4.1.xml")

BGAPI_WIFI_API: Final[ParsedApi] = ParsedApi(BGAPI_WIFI_DEF)

BGAPI_WIFI_ENCODER: Final = serdeser.Serializer([BGAPI_WIFI_API])
BGAPI_WIFI_DECODER: Final = serdeser.Deserializer([BGAPI_WIFI_API])


def decode(deser: serdeser.Deserializer, data: bytes, fromHost: bool = False):
    # First 4 bytes are BGAPI header, rest are payload:
    header, payload = data[:4], data[4:]

    cmdevt, dev_id, payload_len, cls_idx, cmd_idx = deser.parseHeader(header)

    (apicmdevt, headers, params) = deser.parse(
        header, payload, fromHost=fromHost
    )

    msg: BGMsg

    if cmdevt == serdeser.MSG_COMMAND:
        if fromHost:
            # This data was sent from the host, so this is a command sent by
            # the host:
            msg = BGCommand(apicmdevt, params)
        else:
            # This data was sent to the host, so this is a command response:
            msg = BGResponse(apicmdevt, params)
    else:
        # Got event
        msg = BGEvent(apicmdevt, params)

    return msg
