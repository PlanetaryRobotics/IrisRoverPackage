Module IrisBackendv3.transceiver.wifi_transceiver
=================================================
Defines the Transceiver class for interfacing with the rover over Wifi.

By default this side (Ground) acts as the lander and connects to a gateway on
the PM1LWAP hotspot.

Includes any supporting functions necessary for maintaining wifi connection.

@author: Connor W. Colombo (CMU)
@last-updated: 06/22/2023

Functions
---------

    
`coerce_addr(addr: _AddrType) ‑> Tuple[Tuple[int, int, int, int], int]`
:   Standardizes format of an address from UDP layer.

    
`coerce_ip(ip: Any) ‑> Tuple[int, int, int, int]`
:   Converts the given input to an IP type if possible.
    Raises TypeError if not.

    
`ip_to_str(ip: IP) ‑> str`
:   

Classes
-------

`WifiTransceiver(rover_ip: IP | str = (192, 168, 150, 3), rover_port: int = 42000, gateway_ip: IP | str = (192, 168, 150, 254), lander_ip: IP | str = (192, 168, 10, 105), lander_port: int = 43531, inbound_queue_size: int = 200, gateway_send_force: bool = False, endecs: Optional[List[Endec]] = None, pathway: DataPathway = DataPathway.WIRELESS, source: DataSource = DataSource.WIFI_CONNECTION_DIRECT, **kwargs)`
:   Abstract base class for all `Transceivers`. This class handles all
    logic interfacing with `Packet` classes and `Endec`s so the implementation
    of the subclasses can be as simple as possible (just handling how to get
    and send packet bytes.)
    
    Initializes a `WifiTransceiver`.
    
    NOTE: You'll need to supply the appropriate list of `endecs` which were
    used to encode the data that was put into the PCAP.

    ### Ancestors (in MRO)

    * IrisBackendv3.transceiver.transceiver.Transceiver
    * abc.ABC

    ### Class variables

    `gateway_ip: Tuple[int, int, int, int]`
    :

    `gateway_send_force: bool`
    :

    `lander_ip: Tuple[int, int, int, int]`
    :

    `lander_port: int`
    :

    `rover_ip: Tuple[int, int, int, int]`
    :

    `rover_port: int`
    :

    `server_endpoint: IrisBackendv3.transceiver.utils.asyncio_udp.Endpoint | None`
    :

    `server_queue_size: int`
    :

    `server_socket: socket.socket | None`
    :

    ### Methods

    `begin(self) ‑> None`
    :   Initialize any special registers, etc. for this transceiver.
        Can also be used to reset the state of the transceiver.

    `start_server(self) ‑> None`
    :   Starts the UDP Server Endpoint for incoming processing incoming 
        (downlinked) packets.
        
        NOTE `reuse_address=False` to ensure a default is set since having it
        not False is generally an antipattern for UDP and presents a security
        risk on Linux per:
        https://docs.python.org/3/library/asyncio-eventloop.html#asyncio.loop.create_datagram_endpoint