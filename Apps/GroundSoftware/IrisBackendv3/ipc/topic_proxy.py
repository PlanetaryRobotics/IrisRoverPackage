"""Subscriber/Publisher Proxy which allows N publishers to connect to N 
subscribers (without having to worry about the discovery or binding problems).

This proxy binds to both the subscriber and publisher ports and statelessly
forwards messages and un/subscription notices between them. This follows the
XPUB-XSUB topology found here:
https://netmq.readthedocs.io/en/latest/xpub-xsub/.

@author: Connor W. Colombo (CMU)
@last-updated: 04/29/2023
"""
import socket
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
from IrisBackendv3.ipc.logs import logger


class TopicProxy:
    inbound_port: Port
    outbound_port: Port
    context: Context
    inbound_socket: Socket
    outbound_socket: Socket

    def __init__(
        self,
        inbound_port: Port,
        outbound_port: Port
    ) -> None:
        """Binds and subscribes to `inbound_port` and forwards (publishes)
        all messages to `outbound_port` (also bound). Forwards messages
        from all `topics`.
        """
        logger.verbose(
            f"Creating `TopicProxy` with: "
            f"`{inbound_port=}`, `{outbound_port=}`."
        )
        self.inbound_port = inbound_port
        self.outbound_port = outbound_port

        self.context = create_context()
        self.inbound_socket = create_socket(
            self.context,
            SocketType.XSUBSCRIBER,
            ports=inbound_port,
            bind=True
        )
        self.outbound_socket = create_socket(
            self.context,
            SocketType.XPUBLISHER,
            ports=outbound_port,
            bind=True
        )

        # Log address (for connections to network over MiLAN):
        hostname = socket.gethostname()
        addr = socket.gethostbyname(hostname)
        logger.info(f"Created {self} at [{hostname}]({addr}).")

        # Register to make sure `__del__` gets called when program exits:
        # (via interrupt or actual close)
        atexit.register(self.__del__)

    def __del__(self):
        """Close everything on destruction."""
        # Close sockets:
        logger.notice("Closing Proxy's IPC sockets and context . . .")
        if hasattr(self, 'inbound_socket'):
            self.inbound_socket.close()
        if hasattr(self, 'outbound_socket'):
            self.outbound_socket.close()
        # Close context (waiting for all sockets to close first):
        if hasattr(self, 'context'):
            self.context.destroy()  # closes any remaining ctx sockets then `term`
        # Unregister from program exit handler:
        atexit.unregister(self.__del__)

    def __str__(self) -> str:
        return (
            f"{self.__class__.__name__}["
            f"{self.inbound_port}({self.inbound_port.value}=0x{self.inbound_port.value:04X}) "
            f"-> {self.outbound_port}({self.outbound_port.value}=0x{self.outbound_port.value:04X})"
            "]"
        )

    def run(self) -> None:
        logger.notice(f"Starting {self} . . .")
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

    opts = parser.parse_args()

    # Create & Start Proxy:
    proxy = TopicProxy(
        inbound_port=Port[opts.inbound_pub_port],
        outbound_port=Port[opts.outbound_sub_port],
    )
    proxy.run()
