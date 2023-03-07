"""
Simple demo of synchronous publisher over IPC, without using an AppManager.
This demo is designed to be run alongside `ipc_sync_pub.py`.
This demo uses a reverse topology where the **sub** binds the port and listens
to 1 or more pubs but can be switched by flipping the `bind` variable in each
demo.

@author: Connor W. Colombo (CMU)
@last-updated: 03/07/2023
"""
import IrisBackendv3 as IB3
from IrisBackendv3.ipc.messages import DownlinkedPacketsMessage

IB3.init_from_latest()

# Setup IPC:
context = IB3.ipc.create_context()
socket = IB3.ipc.create_socket(
    context,
    IB3.ipc.SocketType.SUBSCRIBER,
    IB3.ipc.Port.TRANSCEIVER,
    bind=True
)
IB3.ipc.subscribe(socket, [IB3.ipc.Topic.DL_PACKETS])

# Read up to 10 packets:
print(f"Reading up to 10 messages of packets from {IB3.ipc.Topic.DL_PACKETS}:")
for _ in range(10):
    raw, msg = IB3.ipc.read_from_as(socket, DownlinkedPacketsMessage)
    packets = msg.content.packets
    print(f"\t{raw.topic=},\t {raw.subtopic_bytes=},\t {len(packets)=}")

# Teardown:
print("Tearing down . . .")
socket.close()
context.destroy()
