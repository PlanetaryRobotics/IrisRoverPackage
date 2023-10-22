"""
Demo of synchronous subscriber app listening for Telemetry (and events) over
IPC using an AppManager.
This demo is designed to be run alongside `make data` (or anything else that
publishes data into the transceiver pipeline).

Make sure to run the infrastructure first:
```
make run-infra
```

@author: Connor W. Colombo (CMU)
@last-updated: 09/23/2023
"""
from typing import cast
import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.codec.payload import TelemetryPayload, EventPayload
from IrisBackendv3.ipc.messages import (
    DownlinkedPayloadsMessage
)
IB3.init_from_latest()

app = ipc.IpcAppHelper("YOUR-APP-NAME")
app.setLogLevel('VERBOSE')

# Setup:
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER_SUB,
        topics=[ipc.Topic.DL_PAYLOADS]
    )
})

# Run:
for _ in range(50):  # read 50 packets worth of data then stop
    ipc_payload = manager.read('sub')
    try:
        msg = ipc.guard_msg(ipc_payload.message,
                            DownlinkedPayloadsMessage)
        payloads = msg.content.payloads
    except Exception as e:
        app.logger.error(
            f"Failed to decode IPC message `{msg}` "
            f"of `{ipc_payload=}` b/c: `{e}`."
        )

    # Print out all Telemetry we received:
    for telem in payloads[TelemetryPayload]:
        telem = cast(TelemetryPayload, telem)

        # Fish out a special telem channel we care about:
        if (telem.module.name, telem.channel.name) == ('WatchdogHeartbeatTvac', 'AdcTempKelvin'):
            app.logger.notice(f"BATTERY TEMP IS: {telem.data - 273.15}ºC")
        else:
            app.logger.verbose(f"GOT: {telem}")

    # Print out all Events we received:
    for event in payloads[EventPayload]:
        event = cast(EventPayload, event)
        # Fish out a special event we care about:
        if (event.module.name, event.event.name) == ('GroundInterface', 'SpecialMessage'):
            app.logger.success(f"Special Message: '{event.formatted_string}'")
        else:
            app.logger.info(f"EVENT: {event}")
