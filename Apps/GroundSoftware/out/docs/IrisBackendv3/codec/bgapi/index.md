Module IrisBackendv3.codec.bgapi
================================

Sub-modules
-----------
* IrisBackendv3.codec.bgapi.custom_listener

Functions
---------

    
`build_command(api: bgapi.apiparser.ParsedApi, class_name: str, cmd_name: str, args: Dict[str, Any]) ‑> bgapi.bglib.BGCommand`
:   Simple way to validate and build a BGCommand with the given class and name with the
    given arg dictionary from the given API.
    
    Example:
    ```py
    build_command(BGAPI_WIFI_API, 'config', 'get_mac', {'hw_interface': 0})
    ```

    
`decode(deser: bgapi.serdeser.Deserializer, data: bytes, fromHost: bool = False)`
:   

    
`encode_command(enc: bgapi.serdeser.Serializer, cmd: bgapi.bglib.BGCommand) ‑> bytes`
:   Simple way to encode a given BGCommand using the given encoder /
    serializer.
    
    Example:
    ```py
    cmd = build_command(BGAPI_WIFI_API, 'config', 'get_mac', {'hw_interface': 0})
    encode_command(BGAPI_WIFI_ENCODER, cmd)
    ```