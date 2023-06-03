"""
Simple demo of synchronous publisher over IPC, without using an AppManager.
This demo sends packets loaded using a `PcapTransceiver`.
This demo is designed to be run alongside `ipc_sync_sub.py`.

Run the Transceiver TopicProxy in a separate terminal before running this
script and keep it running for the duration of this script:
```
make proxies
```

@author: Connor W. Colombo (CMU)
@last-updated: 04/29/2023
"""
import time
import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import DownlinkedPacketsMessage, DownlinkedPacketsContent
from IrisBackendv3.ipc.messages import UplinkPacketRequestMessage, UplinkPacketRequestContent
from IrisBackendv3.transceiver.logs import logger_setConsoleLevel as xcvrLoggerLevel

IB3.init_from_latest()

# Load data:
xcvrLoggerLevel('NOTICE')
xcvr = IB3.transceiver.prebuilts.build_xcvr_by_name(
    'PCAP__RC9_5_3__2MIN_IMU',
    fixed_period_ms=10,  # keep it short so the queue fills up
    log_on_receive=False
)
xcvr.begin()
time.sleep(1)  # wait for XCVR to fill up with some packets
packets = xcvr.read()

# Create socket:
context = IB3.ipc.create_context()
socket = IB3.ipc.create_socket(
    context,
    IB3.ipc.SocketType.PUBLISHER,
    IB3.ipc.Port.TRANSCEIVER_PUB
)

# Wait for sub to connect:
print("Waiting 5 sec  . . .")
time.sleep(5)  # much longer than sub will take... but that's the point

# Send 10 packets:
print("Sending 10 messages to two topics:")
for i in range(10):
    # Send 1 msg to the topic being listened to:
    msg = DownlinkedPacketsMessage(DownlinkedPacketsContent(
        packets=packets[:i]  # Send as many packets as there are
    ))
    ipc.send_to(socket, msg, subtopic=b'packets', topic=ipc.Topic.DL_PACKETS)
    print(f"Sent to {ipc.Topic.DL_PACKETS}: {i} packets.")

    # Send another to a different topic (to make sure it doesn't go through):
    msg2 = UplinkPacketRequestMessage(UplinkPacketRequestContent(
        packet=packets[0]
    ))
    ipc.send_to(socket, msg2, subtopic=b'packets', topic=ipc.Topic.UL_PACKET)
    print(f"Send to {ipc.Topic.UL_PACKET}: 1 packet.")
    # Wait before sending the next one:
    time.sleep(1)

# Teardown:
print("Tearing down . . .")
socket.close()
context.destroy()
