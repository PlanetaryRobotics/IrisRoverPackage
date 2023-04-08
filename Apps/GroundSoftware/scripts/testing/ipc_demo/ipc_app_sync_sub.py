"""
Demo of synchronous subscriber over IPC using an AppManager.
This demo is designed to be run alongside `ipc_sync_pub.py`.
This demo uses a reverse topology where the **sub** binds the port and listens
to 1 or more pubs but can be switched by flipping the `bind` variable in each
demo.

@author: Connor W. Colombo (CMU)
@last-updated: 03/07/2023
"""
import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
IB3.init_from_latest()

# Setup:
manager = ipc.IpcAppManagerSync(socket_specs={
    'sub': ipc.SocketSpec(
        sock_type=ipc.SocketType.SUBSCRIBER,
        port=ipc.Port.TRANSCEIVER,
        topics=[ipc.Topic.DL_PACKETS, ipc.Topic.UL_PACKET],
        bind=True
    )
})

# Run:
for _ in range(10):
    raw, msg = manager.read_msg('sub')
    print(
        f"{raw.topic=}, {raw.subtopic_bytes=} with {msg.content!s}"
    )
