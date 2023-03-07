"""
Simple demo of synchronous publisher over IPC, without using an AppManager.
This demo sends packets loaded using a `PcapTransceiver`.
This demo is designed to be run alongside `ipc_sync_sub.py`.
This demo uses a reverse topology where the **sub** binds the port and listens
to 1 or more pubs but can be switched by flipping the `bind` variable in each
demo.

@author: Connor W. Colombo (CMU)
@last-updated: 03/07/2023
"""
import time
import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import DownlinkedPacketsMessage, DownlinkedPacketsContent
from IrisBackendv3.ipc.messages import UplinkPacketRequestMessage, UplinkPacketRequestContent
from IrisBackendv3.transceiver.logging import logger_setConsoleLevel as xcvrLoggerLevel

IB3.init_from_latest()

# Load data:
xcvrLoggerLevel('NOTICE')
xcvr = IB3.transceiver.prebuilts.build_xcvr_by_name(
    'PCAP-18H',
    packetgap=37000,  # skip first 37000 packets (of 37644)
    fixed_period_ms=10,
    loop=False,
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
    IB3.ipc.Port.TRANSCEIVER,
    bind=False
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
