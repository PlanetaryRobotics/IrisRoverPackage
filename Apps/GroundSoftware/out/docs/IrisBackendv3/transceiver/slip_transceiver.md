Module IrisBackendv3.transceiver.slip_transceiver
=================================================
Defines the Transceiver class for reading telemetry and sending commands over
a serial port. Main use-case is using an RS-422 to Serial dongle to
communicate with the rover over RS-422.

Includes any supporting functions necessary for maintaining serial connection.

@author: Connor W. Colombo (CMU)
@last-updated: 07/03/2022

Functions
---------

    
`list_all_device_info(print_data: bool = True) ‑> List[Dict[str, Any]]`
:   User-helper function that returns (and optionally prints) a list of
    dictionaries of device info for every COM device attached to the system.
    Useful to help operators determine the serial number of a serial device
    (for autodetection).

Classes
-------

`SlipTransceiver(device_sn: str | None = None, device_port: str | None = None, baud: int = 57600, endecs: Optional[List[IrisBackendv3.transceiver.endecs.endec.Endec]] = None, pathway: IrisBackendv3.codec.metadata.DataPathway = DataPathway.WIRED, source: IrisBackendv3.codec.metadata.DataSource = DataSource.UDP_SERIAL_DIRECT, **kwargs)`
:   Abstract base class for all `Transceivers`. This class handles all
    logic interfacing with `Packet` classes and `Endec`s so the implementation
    of the subclasses can be as simple as possible (just handling how to get
    and send packet bytes.)
    
    Initializes a `SlipTransceiver`.
    
    NOTE: Either a serial number for the device, `device_sn`, or a device
    COM port, `device_port` can be given.
    
    NOTE: Since a device's COM port assignment can be transient, `device_sn`
    can be given and the system will attempt to autodetect which port its on
    using USB info. The first COM device found whose serial number matches
    the given `device_sn` will be used.
    
    NOTE: `device_sn` matching is case insensitive and partial matches are
    allowed.
        - The first device found with a case-sensitive match will be
            used.
        - If no direct case-sensitive matches are found, the the first
            device found with a case-insensitive match will be used.
        - If no direct case-insensitive matches are found, the the first
            device found with a case-insensitive partial match will be used.
        - If no partial case-insensitive matches are found, the
            `device_port` will be used, if given, or an exception will be
            raised if not.
    
    NOTE: If both `device_sn` and `device_port` are given, the system will
    search for a device with the given `device_sn` first. If that can't be
    found, it will `warn` and then check  if the given `device_port` is
    available.  If neither is available, an exception will be raised.
    
    NOTE: `SlipTransceiver` uses the `SlipEndec` internally. You **don't**
    need to (and shouldn't) include `SlipEndec` in the `endecs` list.
    
    The following example represents a Transceiver that encodes packets
    with A then B then SLIP (header A goes on first, etc.):
    ```python
    slip_udp = SlipTransceiver(
        device_sn = 'ABC123',
        baud = 57600,
        endecs = [EndecB(), EndecA()]
    )
    ```

    ### Ancestors (in MRO)

    * IrisBackendv3.transceiver.transceiver.Transceiver
    * abc.ABC

    ### Class variables

    `baud: int`
    :

    `serial_port: Optional[serial.serialposix.Serial]`
    :

    ### Instance variables

    `device_port: str | None`
    :

    `device_sn: str | None`
    :

    ### Methods

    `begin(self) ‑> None`
    :   Initialize any special registers, etc. for this transceiver.
        Can also be used to reset the state of the transceiver.

    `connect_serial(self) ‑> None`
    :   Open a serial port connection.