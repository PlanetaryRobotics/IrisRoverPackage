"""
Demo of asynchronous subscriber over IPC using an AppManager.
This demo is designed to be run alongside `ipc_sync_pub.py`.

Run the Transceiver TopicProxy in a separate terminal before running this
script and keep it running for the duration of this script:
```
./run-script.sh IrisBackendv3/ipc/topic_proxy.py -i TRANSCEIVER_PUB -o TRANSCEIVER_SUB
```

@author: Connor W. Colombo (CMU)
@last-updated: 04/29/2023
"""
import asyncio
from typing import ClassVar

import IrisBackendv3 as IB3
import IrisBackendv3.ipc as ipc
from IrisBackendv3.ipc.messages import DownlinkedPacketsMessage

IB3.init_from_latest()


class Sub(ipc.SocketTopicHandlerAsync['Sub']):
    """A demo Subscriber that only cares about 1 topic and ends everything
    after it gets 10 messages."""
    _raise_on_unhandled_topics: ClassVar[bool] = False
    _require_unhandled_topic_handler: ClassVar[bool] = False
    # Decorator shorthand (also plays more nicely with syntax highlighting):
    topic_handler = ipc.SocketTopicHandlerAsync['Sub'].TopicHandlerFactory()

    rx_count: int  # simple example of maintaining an internal state

    def __init__(self, *args, **kwargs) -> None:
        self.rx_count = 0
        super().__init__(*args, **kwargs)

    @topic_handler
    async def downlink_handler(
        self,
        manager: ipc.IpcAppManagerAsync,
        payload: ipc.IpcPayload
    ) -> None:
        # Decode the message:
        msg: ipc.InterProcessMessage = payload.message
        # Make sure we got the message we expected (not just any message):
        msg = ipc.guard_msg(msg, DownlinkedPacketsMessage)
        print(
            f"{payload.topic=}, {payload.subtopic_bytes=} "
            f"with {msg.content.simple_str()}"
        )
        self.rx_count += 1
        if self.rx_count == 10:
            # We're done. Yeet out of here:
            raise ipc.IpcEndAppRequest("Got 10. Thanks!")

    _topic_handlers: ClassVar[ipc.SocketTopicHandlerAsync.TopicHandlerRegistry] = {
        ipc.Topic.DL_PACKETS: downlink_handler
    }


manager = IB3.ipc.IpcAppManagerAsync(socket_specs={
    'sub': IB3.ipc.SocketSpec(
        sock_type=IB3.ipc.SocketType.SUBSCRIBER,
        port=IB3.ipc.Port.TRANSCEIVER_SUB,
        topics=Sub.TOPICS(),
        rx_handler=Sub()
    )
})

asyncio.run(manager.run())
