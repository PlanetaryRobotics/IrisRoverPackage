"""Subscriber/Publisher Proxy which allows N publishers to connect to N 
subscribers (without having to worry about the discovery or binding problems).

This proxy binds to both the subscriber and publisher ports and forwards
statelessly messages and subscription notices between them. This follows the
XPUB-XSUB topology found here: 
https://netmq.readthedocs.io/en/latest/xpub-xsub/.

@author: Connor W. Colombo (CMU)
@last-updated: 04/29/2023
"""
import atexit
import argparse
from typing import List

from IrisBackendv3.ipc.wrapper import (
    Context, Socket, SocketType,
    create_context, create_socket,
    subscribe, start_proxy
)
from IrisBackendv3.ipc.port import Port
from IrisBackendv3.ipc.topics_registry import Topic
# from IrisBackendv3.ipc.logging import logger


class TopicProxy:
    context: Context
    inbound_socket: Socket
    outbound_socket: Socket

    def __init__(
        self,
        inbound_port: Port,
        outbound_port: Port,
        topics: Topic | List[Topic] | None = None
    ) -> None:
        """Binds and subscribes to `inbound_port` and forwards (publishes)
        all messages to `outbound_port` (also bound). Only forwards messages
        from topic in `topics`, unless `topics` is None, in which case messages
        from all `topics` are forwarded.
        """
        self.context = create_context()
        self.inbound_socket = create_socket(
            self.context,
            SocketType.XSUBSCRIBER,
            ports=inbound_port,
            bind=True
        )
        subscribe(self.inbound_socket, topics)
        self.outbound_socket = create_socket(
            self.context,
            SocketType.XPUBLISHER,
            ports=outbound_port,
            bind=True
        )

        # Register to make sure `__del__` gets called when program exits:
        # (via interrupt or actual close)
        atexit.register(self.__del__)

    def __del__(self):
        """Close everything on destruction."""
        # Close sockets:
        # logger.notice("Closing Proxy's IPC sockets and context . . .")
        self.inbound_socket.close()
        self.outbound_socket.close()
        # Close context (waiting for all sockets to close first):
        self.context.destroy()  # closes any remaining ctx sockets then `term`
        # Unregister from program exit handler:
        atexit.unregister(self.__del__)

    def run(self) -> None:
        start_proxy(self.inbound_socket, self.outbound_socket)


# Proxy as a Process:
if __name__ == "__main__":
    # If run as a program, this will launch a topic proxy between the ports
    # with the given names for the topics with the given names (making sure the
    # names are valid first):
    # Get CLI options:
    parser = argparse.ArgumentParser(description=(
        'IRIS Lunar Rover — IPC Topic Proxy — CLI'
    ))
    port_names = [p.name for p in Port]
    parser.add_argument('-i', '--inbound-pub-port',
                        type=str, required=True, choices=port_names,
                        help='Inbound port (for publishers).')
    parser.add_argument('-o', '--outbound-sub-port',
                        type=str, required=True, choices=port_names,
                        help='Outbound port (for subscribers).')

    topic_names = [t.name for t in Topic]
    parser.add_argument('-t', '--topics', required=False, choices=topic_names,
                        nargs='*', default=[],
                        help=(
                            "Topics to forward."
                            "If none given, all topics will be forwarded."
                        ))
    opts = parser.parse_args()

    # Start Proxy:
    topics = None if len(opts.topics) == 0 else [Topic[t] for t in opts.topics]
    # logger.
    proxy = TopicProxy(
        inbound_port=Port[opts.inbound_pub_port],
        outbound_port=Port[opts.outbound_sub_port],
        topics=topics
    )
    proxy.run()
