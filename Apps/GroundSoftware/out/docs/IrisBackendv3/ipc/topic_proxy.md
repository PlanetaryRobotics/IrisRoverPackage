Module IrisBackendv3.ipc.topic_proxy
====================================
Subscriber/Publisher Proxy which allows N publishers to connect to N 
subscribers (without having to worry about the discovery or binding problems).

This proxy binds to both the subscriber and publisher ports and statelessly
forwards messages and un/subscription notices between them. This follows the
XPUB-XSUB topology found here:
https://netmq.readthedocs.io/en/latest/xpub-xsub/.

@author: Connor W. Colombo (CMU)
@last-updated: 04/29/2023

Classes
-------

`TopicProxy(inbound_port: IrisBackendv3.ipc.port.Port, outbound_port: IrisBackendv3.ipc.port.Port)`
:   Binds and subscribes to `inbound_port` and forwards (publishes)
    all messages to `outbound_port` (also bound). Forwards messages
    from all `topics`.

    ### Class variables

    `context: zmq.sugar.context.Context`
    :

    `inbound_port: IrisBackendv3.ipc.port.Port`
    :

    `inbound_socket: zmq.sugar.socket.Socket`
    :

    `outbound_port: IrisBackendv3.ipc.port.Port`
    :

    `outbound_socket: zmq.sugar.socket.Socket`
    :

    ### Methods

    `run(self) ‑> None`
    :