import os

from typing import Any, Final, Dict, cast

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
from IrisBackendv3.codec.bgapi.custom_listener import BGLibListener

# Also expose API paths:
BGAPI_DIR: Final = os.path.dirname(os.path.realpath(__file__))
BGAPI_WIFI_DEF: Final = os.path.join(BGAPI_DIR, "./bgapi_wifi_1.4.1.xml")

BGAPI_WIFI_API: Final[ParsedApi] = ParsedApi(BGAPI_WIFI_DEF)

BGAPI_WIFI_ENCODER: Final = serdeser.Serializer([BGAPI_WIFI_API])
BGAPI_WIFI_DECODER: Final = serdeser.Deserializer([BGAPI_WIFI_API])


def build_command(api: ParsedApi, class_name: str, cmd_name: str, args: Dict[str, Any]) -> BGCommand:
    """Simple way to validate and build a BGCommand with the given class and name with the
    given arg dictionary from the given API.

    Example:
    ```py
    build_command(BGAPI_WIFI_API, 'config', 'get_mac', {'hw_interface': 0})
    ```
    """
    # Grab the API Command definition:
    if class_name not in api:
        raise ValueError(
            f"Failed to build command "
            f"api={api.name}, class={class_name}, cmd={cmd_name}, args={args} "
            f"because class not found in API."
        )
    api_class_def = api[class_name]
    if cmd_name not in api_class_def.commands:
        raise ValueError(
            f"Failed to build command "
            f"api={api.name}, class={class_name}, cmd={cmd_name}, args={args} "
            f"because command not found in class."
        )
    api_cmd_def = api_class_def.commands[cmd_name]

    # Validate all args:
    if any(p.name not in args for p in api_cmd_def.params):
        raise ValueError(
            f"Failed to build command "
            f"api={api.name}, class={class_name}, cmd={cmd_name}, args={args} "
            f"b/c command requires params {[p.name for p in api_cmd_def.params]} "
            f"and at least one param name wasn't found in the dict of args given."
        )
    # Create list of params in the right order:
    params = [args[p.name] for p in api_cmd_def.params]

    # Create and return BGCommand:
    return BGCommand(api_cmd_def, params)


def encode_command(enc: serdeser.Serializer, cmd: BGCommand) -> bytes:
    """Simple way to encode a given BGCommand using the given encoder /
    serializer.

    Example:
    ```py
    cmd = build_command(BGAPI_WIFI_API, 'config', 'get_mac', {'hw_interface': 0})
    encode_command(BGAPI_WIFI_ENCODER, cmd)
    ```
    """
    return cast(bytes, enc.command(cmd._device_name, cmd._class_name, cmd._msg_name, cmd))


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
