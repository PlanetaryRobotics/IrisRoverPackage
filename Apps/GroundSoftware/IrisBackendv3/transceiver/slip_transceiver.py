"""
Defines the Transceiver class for reading telemetry and sending commands over
a serial port. Main use-case is using an RS-422 to Serial dongle to
communicate with the rover over RS-422.

Includes any supporting functions necessary for maintaining serial connection.

@author: Connor W. Colombo (CMU)
@last-updated: 07/03/2022
"""
from typing import Any, Optional, Callable, Dict, Deque, List, Type, cast

import serial  # type: ignore # no type hints
from serial.tools import list_ports, list_ports_common  # type: ignore
from collections import deque
import re

from .transceiver import Transceiver
from .endec import Endec, SlipEndec
from .logging import logger
from .exceptions import TransceiverConnectionException, TransceiverDecodingException

from IrisBackendv3.codec.packet import Packet, IrisCommonPacket
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.metadata import DataPathway, DataSource
from IrisBackendv3.utils.basic import type_guard_argument


def list_all_device_info(print_data: bool = True) -> List[Dict[str, Any]]:
    """ User-helper function that returns (and optionally prints) a list of
    dictionaries of device info for every COM device attached to the system.
    Useful to help operators determine the serial number of a serial device
    (for autodetection).
    """
    dd = [cp.__dict__ for cp in list_ports.comports()]
    if print_data:
        print(dd)
    return dd


class SlipTransceiver(Transceiver):
    # Device Serial
    _device_sn: Optional[str]  # A7035PDL on dev harness
    _device_port: Optional[str]
    serial_port: Optional[serial.Serial]
    baud: int

    # Slip data:
    _recv_buffer: bytes

    # Public getter, private setter:
    @property
    def device_port(self) -> Optional[str]: return self._device_port
    @property
    def device_sn(self) -> Optional[str]: return self._device_sn

    @staticmethod
    def _find_serial_port(
        ports: List[list_ports_common.ListPortInfo],
        filter_func: Callable[[list_ports_common.ListPortInfo], bool]
    ) -> Optional[list_ports_common.ListPortInfo]:
        """ Internal helper function to return the first port that in the given
        list of `ports` that satisfies the given `filter_func` lambda.
        Produces warnings if too many matches are found.

        NOTE: To get `ports`, use `list_ports.comports()`.
        """
        matches = [cp for cp in ports if filter_func(cp)]
        if len(matches) > 0:
            # a match was found
            if len(matches) > 1:
                # warn that too many matches were found and we had to
                # arbitrarily pick the first match:
                logger.warn(
                    "For a `SlipTransceiver`, multiple matches were found for the "
                    "given information. Consider using a more specific specifier. "
                    "The following matches were found: "
                    f"`{[cp.__dict__ for cp in matches]}`."
                    f"The following port was used: `{matches[0].__dict__}`."
                )
            return matches[0]
        else:
            return None

    def _detect_device(
        self,
        device_sn: Optional[str],
        device_port: Optional[str]
    ) -> bool:
        """
        Internal helper function to autodetect the specified device and update
        the `_device_sn` and `_device_port`.

        See `__init__` for notes on how this detection process works.

        Returns whether this was successful or not.
        """

        # Find the appropriate serial device using the given info:
        # First grab all available ports (since we might be using this a lot):
        all_ports = list_ports.comports()
        port_info: Optional[list_ports_common.ListPortInfo] = None
        if device_sn is not None:
            # Validate input:
            device_sn = cast(str, type_guard_argument(str, 'device_sn', device_sn,
                                                      calling_function_name='SlipTransceiver.__init__'))
            # Search for the associated COM port:
            # Check for all direct case-sensitive matches first:
            port_info = self._find_serial_port(
                all_ports,
                lambda cp: (
                    cp.serial_number is not None
                    and device_sn == cp.serial_number
                )
            )
            if port_info is None:
                # No direct case-sensitive match was found, then check for all
                # direct case-insensitive matches:
                port_info = self._find_serial_port(
                    all_ports,
                    lambda cp: (
                        cp.serial_number is not None
                        and str(device_sn).lower() == cp.serial_number.lower()
                    )
                )
            if port_info is None:
                # No direct case-insensitive match was found, then check for all
                # partial case-insensitive matches:
                port_info = self._find_serial_port(
                    all_ports,
                    lambda cp: (
                        cp.serial_number is not None
                        and str(device_sn).lower() in cp.serial_number.lower()
                    )
                )
            if port_info is None:
                logger.warn(
                    "`SlipTransceiver` was initialized with `device_sn = "
                    f"{device_sn}` but no partial case-insensitive matches "
                    "were found among all COM devices. "
                    "Available COM devices were: "
                    f"`{[cp.__dict__ for cp in all_ports]}`. "
                )

        if port_info is None and device_port is not None:
            # No device was automatically detected by serial number (if given)
            # but a `device_port` was given so just use the given port:
            port_info = self._find_serial_port(
                all_ports,
                lambda cp: (
                    cp.device is not None
                    and device_port == cp.device
                )
            )
            if port_info is None:
                logger.warn(
                    "`SlipTransceiver` was initialized with `device_port = "
                    f"{device_port}` but no matching device was found among "
                    "all COM devices. "
                    "Available COM devices were: "
                    f"`{[cp.__dict__ for cp in all_ports]}`. "
                )

        if port_info is None:
            # No match was found so just save the given info (so a detection
            # can be re-attempted later):
            self._device_sn = device_sn
            self._device_port = device_port
            return False
        else:
            # a match was found, use it:
            self._device_sn = port_info.serial_number
            self._device_port = port_info.device
            return True

    def _init_slip_data(self) -> None:
        """ (Re)initializes SLIP data."""
        self._recv_buffer = b''

    def __init__(
        self,
        device_sn: Optional[str] = None,  # **DON'T** supply defaults here
        device_port: Optional[str] = None,
        baud: int = 57600,
        endecs: Optional[List[Endec]] = None,
        pathway: DataPathway = DataPathway.WIRED,
        source: DataSource = DataSource.UDP_SERIAL_DIRECT,
        **kwargs
    ) -> None:
        """ Initializes a `SlipTransceiver`.

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
        """
        if endecs is None:
            endecs = []
        super().__init__(
            endecs=cast(List[Endec], [SlipEndec()]) + endecs,  # wrap SLIP last
            pathway=pathway,
            source=source,
            **kwargs  # fwd all other kwargs to parent
        )

        self._init_slip_data()

        # Validate primitive inputs:
        self.baud = type_guard_argument(int, 'baud', baud,
                                        calling_function_name='SlipTransceiver.__init__')

        # Initialize serial object:
        self.serial_port = None

        # Autodetect serial port if a serial number was given:
        if device_sn is not None and device_port is not None:
            logger.info(
                "`SlipTransceiver` was initialized with both `device_sn` "
                f"({device_sn}) and `device_port` ({device_port}). "
                "This information is redundant. "
                "The system will search for a device with the given `device_sn` "
                "first. If that can't be found, it will `warn` and then check "
                "if the given `device_port` is available. "
                "If neither is available, an exception will be raised. "
                "If you don't want this behavior, only supply **either** "
                "`device_sn` **xor** `device_port` to the `SlipTransceiver`."
            )

        if not self._detect_device(device_sn, device_port):
            raise TransceiverConnectionException(
                "`SlipTransceiver` was failed to detect a device when "
                f"initialized with `device_sn={device_sn}` and "
                f"`device_port={device_port}`."
            )

    def connect_serial(self) -> None:
        """
        Open a serial port connection.
        """
        # Attempt to auto-detect device:
        self._detect_device(self.device_sn, self.device_port)
        if self.device_port is None:
            logger.error(
                f"Failed to detect serial device in given: "
                f"`device_sn = {self.device_sn}`, "
                f"`device_port={self.device_port}`"
            )

        try:
            # Open serial port:
            self.serial_port = serial.Serial(
                self.device_port, self.baud
            )

            # NOTE: mypy complains that `logger` doesn't have a member `success` but it does
            logger.success("Connection Success!")  # type: ignore
        except serial.SerialException as e:
            logger.error(
                "Failed to connect to serial device with:"
                f"`device_sn = {self.device_sn}`, "
                f"`device_port={self.device_port}`. "
                "Is the device serial number / port right? "
                f"Check the connection? Original error: {e}",
            )

    def begin(self) -> None:
        """ Initialize any special registers, etc. for this transceiver.
        Can also be used to reset the state of the transceiver.
        """
        super().begin()

        # (Re)Initialize serial connection:
        self.connect_serial()

        # (Re)Initialize SLIP data (if we're starting a new connection session,
        # pre-existing data streams are done):
        self._init_slip_data()

    def _read_all_raw_bytes_from_serial(self) -> bytes:
        """ Reads as many raw bytes from the connected serial line as are
        available.

        NOTE: The bytes returned are raw (**NOT** decoded from SLIP) and the
        start/end **DOESN'T** necessarily align with SLIP packet boundaries.
        """
        # Attempt to connect if not connected:
        if self.serial_port is None:
            self.connect_serial()

        # Grab all data available on the serial interface:
        if self.serial_port is None:
            logger.error(
                "Failed to connect to serial device when trying to read "
                "downlinked bytes."
            )
            # The show must go on (Transceiver is top level, so we can't just
            # exception out) so just return nothing b/c nothing is available:
            data = b''
        else:
            data = self.serial_port.read_all()

        return data

    def _send_raw_bytes_over_serial(self, data: bytes) -> bool:
        """ Sends the given raw bytes over the connected serial line.

        Returns whether the send was successful.

        NOTE: No encoding happens here. So, the bytes given should already be
        encoded with SLIP, etc.
        """
        success = False

        # Attempt to connect if not connected:
        if self.serial_port is None:
            self.connect_serial()

        # Grab all data available on the serial interface:
        if self.serial_port is None:
            logger.error(
                "Failed to connect to serial device when trying to write "
                "bytes to uplink."
            )
        else:
            self.serial_port.write(bytes(data))
            success = True

        return success

    def _downlink_byte_packets(self) -> List[bytes]:
        """ Reads all **complete** packets of bytes from the connected serial
        interface using SLIP (Serial Line Internet Protocol).
        This SLIP implementation is designed according to RFC-1055 and based on
        the implementation in `sliplib 0.6.2` by Ruud de Jong.

        NOTE: No decoding occurs with the `endecs` here, just transmits the
        packet of bytes given.

        NOTE: All byte_packets returned are complete SLIP packets; HOWEVER,
        they have **NOT** yet been decoded with SLIP (that's done using the
        `SlipEndec` in `Transceiver` since it was added to `endecs` in
        `SlipTransceiver.__init__`.)

        Returns a list of all complete packets received, as bytes. If no
            **complete** packets were received, this list is empty.
        """
        # Read any available data:
        data = self._read_all_raw_bytes_from_serial()

        if isinstance(data, int):
            data = bytes((data,))

        # Empty data indicates that the data reception is complete.
        # To force a buffer flush, an END byte is added, so that the
        # current contents of _recv_buffer will form a complete message.
        if len(data) == 0:
            data = SlipEndec.B_END

        self._recv_buffer += data

        # All byte packets extracted during this call:
        _byte_packets: Deque[bytes] = deque()

        # The following situations can occur:
        #
        #  1) _recv_buffer is empty or contains only END bytes --> no packets available
        #  2) _recv_buffer contains non-END bytes -->  packets are available
        #
        # Strip leading END bytes from _recv_buffer to avoid handling empty _byte_packets.
        self._recv_buffer = self._recv_buffer.lstrip(SlipEndec.B_END)
        if self._recv_buffer:
            # The _recv_buffer contains non-END bytes.
            # It is now split on sequences of one or more END bytes.
            # The trailing element from the split operation is a possibly incomplete
            # packet; this element is therefore used as the new _recv_buffer.
            # If _recv_buffer contains one or more trailing END bytes,
            # (meaning that there are no incomplete packets), then the last element,
            # and therefore the new _recv_buffer, is an empty bytes object.
            _byte_packets.extend(
                re.split(SlipEndec.B_END + b'+', self._recv_buffer)
            )
            self._recv_buffer = _byte_packets.pop()

        # Return the extracted complete packets to be processed:
        return [*_byte_packets]

    def _uplink_byte_packet(
        self,
        packet_bytes: bytes,
        **_
    ) -> bool:
        """ Transmits the given packet of bytes on this `Transceiver`'s uplink
        transmission line.

        NOTE: No encoding occurs with the `endecs` here, just transmits the
        bytes as given.

        NOTE: This expects the given `packet_bytes` to have **ALREADY** been
        SLIP encoded by `Transceiver` since `SlipEndec` was added to `endecs` in
        `SlipTransceiver.__init__`.)

        NOTE: `**uplink_metadata` is given in the superclass and contains any
        special data needed by methods further down the uplink pipeline, but is
        unused by `SlipTransceiver` since it has no special metadata.

        Returns whether the uplink was successful.
        """
        return self._send_raw_bytes_over_serial(packet_bytes)
