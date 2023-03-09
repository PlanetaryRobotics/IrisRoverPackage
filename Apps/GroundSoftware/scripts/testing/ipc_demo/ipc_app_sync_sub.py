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
IB3.init_from_latest()

# Setup:
manager = IB3.ipc.IpcAppManagerSync(socket_specs={
    'sub': IB3.ipc.SocketSpec(
        sock_type=IB3.ipc.SocketType.SUBSCRIBER,
        port=IB3.ipc.Port.TRANSCEIVER,
        topics=[IB3.ipc.Topic.DL_PACKETS],
        bind=True
    )
})

# Run:
for _ in range(10):
    raw, msg = manager.read_msg('sub')
    print(
        f"{raw.topic=}, {raw.subtopic_bytes=} with {msg.content.simple_str()}"
    )
