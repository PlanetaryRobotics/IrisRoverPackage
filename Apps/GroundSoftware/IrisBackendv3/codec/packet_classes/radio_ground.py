"""
Implementation of Radio-Ground Packet used by data sent directly from the
Radio's internal MCU to Ground (over WiFi).

The Standard formatting followed by all Radio-Ground messages is:
  - "RAD:" header
  - [Message Type]: Sequence of ASCII characters defining message type
  - "|"
  - A Series of >=0 fields.
      - Each must be separated by "|" (final field is not followed by a "|")
      - Field content itself must not contain the "|" character.
      - Any arbitrary raw hex sent instead of ASCII characters in a field must be directly preceded by an "h" and must either:
          - Have a known / fixed length (i.e. this field is always X bytes)
          - If variable in length, be the final field in the message (so the field's length doesn't need to be transmitted)

Note: All raw data buffers will be little-endian.

Note: this is all going to be wrapped inside a UDP packet, so we don't need
length information, CRC, etc. in the header of this message.

@author: Connor W. Colombo (CMU)
@last-updated: 09/17/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations
from os import supports_bytes_environ

import scapy.all as scp  # type: ignore

from dataclasses import dataclass
from collections import OrderedDict
from typing import List, Any, Optional, Final, Protocol, Dict, ClassVar, cast, Union, Type

from IrisBackendv3.utils.basic import full_dict_spec_check
from IrisBackendv3.data_standards.module import Module, TelemetryChannel, Event, Argument
from IrisBackendv3.data_standards.fsw_data_type import FswDataType

from .packet import Packet, CT
from ..payload import EventPayload, TelemetryPayload, TelemetryPayloadInterface
from ..payload_collection import EnhancedPayloadCollection
from ..fsw_data_codec import (
    decode as fsw_data_decode,
    encode as fsw_data_encode,
    StringPacker as FswStringPacker
)

from ..settings import ENDIANNESS_CODE, settings
from ..exceptions import PacketDecodingException
from ..logging import logger

# Fixed header that starts off all Radio-Ground packets:
RADIO_GROUND_FIXED_HEADER: Final[bytes] = b'RAD:'

# Name of the module this maps to:
RADIO_GROUND_MODULE: Final[str] = 'RadioGround'

# Number of milliseconds per Radio timer tick (baked into Radio-firmware):
RADIO_TICK_DURATION_MS: Final[int] = 5000


def grab_event_arg(event: Event, name: str) -> Argument:
    # Helper function that grabs the arg from the Event with the given name
    return next((a for a in event.args if a.name == name))


def parse_event_args(event: Event, args: Dict[str, bytes]) -> Dict[str, Any]:
    # Helper function that parses all argument bytes according to the
    # corresponding argument's datatype
    return {
        n: fsw_data_decode(grab_event_arg(event, n).datatype, v)
        for n, v in args.items()
    }


def decode_radio_downlink_str(str_data: bytes) -> str:
    """
    Decodes a string sent as part of a Radio packet and makes it
    human-readable. We do this separately from the normal FPrime decoding
    since, for efficiency reasons, the Radio doesn't encode its strings with
    the FPrime standard (since it doesn't run FPrime):
    """
    # Escape any non-unicode hex in the string to make it human-readable
    # and then convert back to bytes:
    return FswStringPacker.decode_str(str_data)


class RadioGroundSubpacketHandler(Protocol):
    """Structural Typing Protocol defining the structure of the callbacks that
    handle converting the fields of a RadioGroundSubpacket to a collection of
    Payloads."""

    def __call__(self, **fields: bytes) -> EnhancedPayloadCollection: ...


def handle_heartbeat(**fields: bytes) -> EnhancedPayloadCollection:
    payloads = EnhancedPayloadCollection()

    # Grab the module:
    module = settings['STANDARDS'].modules[RADIO_GROUND_MODULE]

    # Extract the time:
    # Note: Radio ints are always little-endian:
    uptime_ticks = int.from_bytes(
        fields['UptimeTicks'], 'little'  # b'\x7b\x00\x00\x00' -> 123
    )
    uptime_ms = uptime_ticks * RADIO_TICK_DURATION_MS

    def telem(name: str, val: Any) -> TelemetryPayload:
        # Build standard Heartbeat TelemetryPayload:
        return TelemetryPayload(
            module_id=module.ID,
            channel_id=module.telemetry[name].ID,
            data=val,
            timestamp=uptime_ms,
            raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
        )

    # Extract data from packet (convert bytes to actual values) and Build TelemetryPayloads:

    rssi = -int(fields['Rssi'].decode('utf-8'), 10)  # b'031' -> -31
    payloads.append(telem('Rssi', rssi))

    state_enum = module.telemetry['StateAbbr'].enum
    state_abbr = fields['StateAbbr'].decode('utf-8')  # b'UDPC' -> 'UDPC'
    if state_abbr not in [ei.name for ei in state_enum]:
        # Given abbreviation doesn't match a known value. Flag it as corrupt:
        state_abbr = 'CRPT'
    payloads.append(telem('StateAbbr', state_abbr))

    hrs_part, remainder = divmod(uptime_ms, (60*60*1000))  # 7263023 -> 2,63023
    mins_part, remainder = divmod(remainder, (60*1000))  # 63023 -> 1, 3023
    sec_part, ms_part = divmod(remainder, 1000)  # 3023 -> 3, 23
    uptime_str = f"{hrs_part:02d}h:{mins_part:02d}m:{sec_part:02d}s"
    payloads.append(telem('UptimeTicks', uptime_ticks))
    payloads.append(telem('UptimeMs', uptime_ms))

    rx_count = int.from_bytes(
        fields['UdpRxPacketCount'], 'little'  # b'\x7b\x00\x00\x00' -> 123
    )
    payloads.append(telem('UdpRxPacketCount', rx_count))

    bad_herc_count = int.from_bytes(
        fields['BadHercPacketCount'], 'little'  # b'\x7b\x00\x00\x00' -> 123
    )
    payloads.append(telem('BadHercPacketCount', bad_herc_count))

    ilock_semaphore = int.from_bytes(
        fields['InterlockCountingSemaphore'], 'little'  # b'\x7b' -> 123
    )
    payloads.append(
        telem('HerculesUdpInterlockCountingSemaphore', ilock_semaphore))

    # Build EventPayload:
    payloads.append(
        EventPayload(
            module_id=module.ID,
            event_id=module.events['Heartbeat'].ID,
            args={
                'state_abbr': state_abbr,
                'rssi': rssi,
                'uptime_str': uptime_str
            },
            timestamp=uptime_ms,
            raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
        )
    )

    return payloads


def handle_echo(**fields: bytes) -> EnhancedPayloadCollection:
    payloads = EnhancedPayloadCollection()

    # Grab the module and event:
    module = settings['STANDARDS'].modules[RADIO_GROUND_MODULE]
    event = module.events['Echo']

    # Parse string arg:
    parsed_args: Dict[str, Any] = dict()
    parsed_args['echoed_text'] = decode_radio_downlink_str(
        fields['echoed_text']
    )

    # Build EventPayload:
    payloads.append(EventPayload(
        module_id=module.ID,
        event_id=event.ID,
        args=parsed_args,
        timestamp=0,  # doesn't contain any time information
        raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
    ))

    return payloads


def handle_cmd_cb(event_name: str, **fields: bytes) -> EnhancedPayloadCollection:
    """General handler for all Radio command callbacks:"""
    payloads = EnhancedPayloadCollection()

    # Grab the module and event:
    module = settings['STANDARDS'].modules[RADIO_GROUND_MODULE]
    event = module.events[event_name]

    parsed_args: Dict[str, Any] = dict()

    # Parse string arg:
    parsed_args['string_of_command_data'] = decode_radio_downlink_str(
        fields['command_data']
    )

    # Parse special values (those that don't follow FPrime standard):
    command_arg = grab_event_arg(event, 'command')
    command_id = int.from_bytes(fields['command_id'], 'little')
    try:
        parsed_args['command'] = command_arg.get_enum_name(command_id)
    except ValueError:
        # Value not in enum. Flag it:
        parsed_args['command'] = 'CORRUPTED'

    # Build EventPayload:
    payloads.append(EventPayload(
        module_id=module.ID,
        event_id=event.ID,
        args=parsed_args,
        timestamp=0,  # doesn't contain any time information
        raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
    ))

    return payloads


def handle_cmd_ack(**fields: bytes) -> EnhancedPayloadCollection:
    return handle_cmd_cb('GotCommand', **fields)


def handle_cmd_done(**fields: bytes) -> EnhancedPayloadCollection:
    return handle_cmd_cb('DidCommand', **fields)


def handle_cmd_bad(**fields: bytes) -> EnhancedPayloadCollection:
    return handle_cmd_cb('BadCommand', **fields)


def handle_bad_herc(**fields: bytes) -> EnhancedPayloadCollection:
    payloads = EnhancedPayloadCollection()

    # Grab the module and event:
    module = settings['STANDARDS'].modules[RADIO_GROUND_MODULE]
    event = module.events['BadHerculesPacket']

    parsed_args: Dict[str, Any] = dict()

    # Parse special values (those that don't follow FPrime standard):
    issue_arg = grab_event_arg(event, 'issue')
    issue_id = int.from_bytes(fields['issue'], 'little')
    try:
        parsed_args['issue'] = issue_arg.get_enum_name(issue_id)
    except ValueError:
        # Value not in enum. Flag it:
        parsed_args['issue'] = 'OTHER__UNEXPECTED'

    # Build EventPayload:
    payloads.append(EventPayload(
        module_id=module.ID,
        event_id=event.ID,
        args=parsed_args,
        timestamp=0,  # doesn't contain any time information
        raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
    ))

    return payloads


def handle_bad_endpoint(**fields: bytes) -> EnhancedPayloadCollection:
    payloads = EnhancedPayloadCollection()
    args: Dict[str, bytes] = dict()

    # Grab the module and event:
    module = settings['STANDARDS'].modules[RADIO_GROUND_MODULE]
    event = module.events['BadEndpointData']

    # Map field raw data to event args:
    args['endpoint'] = fields['endpoint']

    # Parse all arguments according to datastandards:
    parsed_args = parse_event_args(event, args)

    # Parse special values (those that don't follow FPrime standard):
    issue_arg = grab_event_arg(event, 'issue')
    issue_id = int.from_bytes(fields['issue'], 'little')
    try:
        parsed_args['issue'] = issue_arg.get_enum_name(issue_id)
    except ValueError:
        # Value not in enum. Flag it:
        parsed_args['issue'] = 'OTHER__UNEXPECTED'

    # Build EventPayload:
    payloads.append(EventPayload(
        module_id=module.ID,
        event_id=event.ID,
        args=parsed_args,
        timestamp=0,  # doesn't contain any time information
        raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
    ))

    return payloads


def handle_bad_udp(**fields: bytes) -> EnhancedPayloadCollection:
    payloads = EnhancedPayloadCollection()
    args: Dict[str, bytes] = dict()

    # Grab the module and event:
    module = settings['STANDARDS'].modules[RADIO_GROUND_MODULE]
    event = module.events['BadUdp']

    # Map field raw data to event args:
    args['port'] = fields['port']

    # Convert IP bytes to string (b'\x01\x01\xA8\xC0' -> '192.168.001.001')
    ip_str = '.'.join(f'{b:03d}' for b in fields['ip'][::-1])
    # ... then back to bytes, following FPrime standard, so it can be format
    # checked by the pipeline:
    args['ip'] = fsw_data_encode(FswDataType.STRING15, ip_str.encode('ascii'))

    # Parse all arguments according to datastandards:
    parsed_args = parse_event_args(event, args)

    # Parse special values (those that don't follow FPrime standard):
    issue_arg = grab_event_arg(event, 'issue')
    issue_id = int.from_bytes(fields['issue'], 'little')
    try:
        parsed_args['issue'] = issue_arg.get_enum_name(issue_id)
    except ValueError:
        # Value not in enum. Flag it:
        parsed_args['issue'] = 'NOTHING__THIS_SHOULD_NOT_HAPPEN'

    # Build EventPayload:
    payloads.append(EventPayload(
        module_id=module.ID,
        event_id=event.ID,
        args=parsed_args,
        timestamp=0,  # doesn't contain any time information
        raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
    ))

    return payloads


def handle_critical_reset(**fields: bytes) -> EnhancedPayloadCollection:
    payloads = EnhancedPayloadCollection()

    # Grab the module and event:
    module = settings['STANDARDS'].modules[RADIO_GROUND_MODULE]
    event = module.events['CriticalReset']

    parsed_args: Dict[str, Any] = dict()

    # Parse special values (those that don't follow FPrime standard):
    issue_arg = grab_event_arg(event, 'issue')
    issue_id = int.from_bytes(fields['issue'], 'little')
    try:
        parsed_args['issue'] = issue_arg.get_enum_name(issue_id)
    except ValueError:
        # Value not in enum. Flag it:
        parsed_args['issue'] = 'OTHER__UNEXPECTED'

    # Build EventPayload:
    payloads.append(EventPayload(
        module_id=module.ID,
        event_id=event.ID,
        args=parsed_args,
        timestamp=0,  # doesn't contain any time information
        raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
    ))

    return payloads


def handle_critical_reset_swe(**fields: bytes) -> EnhancedPayloadCollection:
    payloads = EnhancedPayloadCollection()
    args: Dict[str, bytes] = dict()

    # Grab the module and event:
    module = settings['STANDARDS'].modules[RADIO_GROUND_MODULE]
    event = module.events['CriticalResetDueToSoftwareException']

    # Map field raw data to event args:
    args['memory_address'] = fields['memory_address']
    args['exception_type'] = fields['exception_type']

    # Parse all arguments according to datastandards:
    parsed_args = parse_event_args(event, args)

    # Build EventPayload:
    payloads.append(EventPayload(
        module_id=module.ID,
        event_id=event.ID,
        args=parsed_args,
        timestamp=0,  # doesn't contain any time information
        raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
    ))

    return payloads


@dataclass
class RadioGroundSubpacketField:
    # Metadata about a Field in a RadioGroundSubpacket

    # Sequence of bytes that starts off this field and separates it from the previous
    initiator: bytes
    # Length of field AFTER the initiator, or -1 for variable length (only allowed on final field).
    length: int


@dataclass
class RadioGroundSubpacketDefinition:
    # Key data for each subtype of RadioGroundPacket:

    handler: RadioGroundSubpacketHandler
    # TOTAL length of packet, including headers and all (or -1 if unbounded):
    length: int
    prefix: bytes
    fields: OrderedDict[str, RadioGroundSubpacketField]

    def parse(self, data: bytes) -> EnhancedPayloadCollection:
        """Parses the given data into Payloads using the handler after grabbing
        the component fields from the data.

        NOTE: This assumes the data has already been checked with `is_valid`.
        """
        return self.handler(**self.get_field_data(data))

    def get_field_data(self, data: bytes) -> Dict[str, bytes]:
        """Splits the given data up into fields.
        Returns a map from each field name found to the bytes in that field,
        excluding the initiator.
        """

        field_data: Dict[str, bytes] = dict()

        """ Find all fields by:
        - Start after the header and prefix
        - Look for the first separator
        - Grab the next field.length bytes
        - Advance head by number of bytes consumed
        - Repeat

        ^ Using this method and not just data.split(b'|') prevents errant
        splits when a field contains b'|' as a byte.
        """
        # ignore the first section (that's the header):
        head_idx = len(RADIO_GROUND_FIXED_HEADER) + len(self.prefix)
        for field_name, field_def in self.fields.items():
            field_start = data[head_idx:].find(field_def.initiator)
            if field_start != -1:
                # field found
                # Advance head by initiator bytes (we've "consumed" those):
                head_idx += len(field_def.initiator)
                # Grab the field data:
                start_idx = head_idx + field_start
                if field_def.length != -1:
                    # fixed-length field
                    end_idx = start_idx + field_def.length
                    field_data[field_name] = data[start_idx: end_idx]
                else:
                    # variable-length field (final field):
                    field_data[field_name] = data[start_idx:]
                # Advance head by number of bytes consumed
                head_idx += len(field_data[field_name])

        return field_data

    def is_valid(self, data: bytes) -> bool:
        """Returns whether the given data constitutes a valid instance of this
        Radio-Ground subpacket."""
        valid: bool = True
        hdr_len = len(RADIO_GROUND_FIXED_HEADER)

        # Check that it has the correct/enough length:
        valid = valid & (
            # has the correct length:
            len(data) == self.length or
            # OR we ignore length if it's unbounded (-1) and it has enough length to be parsed:
            self.length == -1 and len(data) >= (hdr_len + len(self.prefix))
        )

        # Check that it has the header:
        if valid:
            valid = valid & (data[:hdr_len] == RADIO_GROUND_FIXED_HEADER)

        # Check that it has the correct prefix:
        if valid:
            valid = valid & (
                data[hdr_len:hdr_len+len(self.prefix)] == self.prefix
            )

        # Parser is able to find all fields:
        if valid:
            valid = valid & (
                len(self.get_field_data(data)) == len(self.fields)
            )

        return valid


# Definitions for all Radio-Ground Subpackets:
RADIO_GROUND_SUBPACKETS: Final[Dict[str, RadioGroundSubpacketDefinition]] = dict(
    heartbeat=RadioGroundSubpacketDefinition(handle_heartbeat, 42, b'HB', OrderedDict([
        ('Rssi', RadioGroundSubpacketField(b'|R-', 3)),
        ('StateAbbr', RadioGroundSubpacketField(b'|', 4)),
        ('UptimeTicks', RadioGroundSubpacketField(b'|Th', 4)),
        ('UdpRxPacketCount', RadioGroundSubpacketField(b'|Rh', 4)),
        ('BadHercPacketCount', RadioGroundSubpacketField(b'|Bh', 4)),
        ('InterlockCountingSemaphore', RadioGroundSubpacketField(b'|Ih', 1))
    ])),
    echo=RadioGroundSubpacketDefinition(handle_echo, -1, b'ECHO', OrderedDict([
        ('echoed_text', RadioGroundSubpacketField(b'|h', -1))
    ])),
    cmd_ack=RadioGroundSubpacketDefinition(handle_cmd_ack, -1, b'ACK', OrderedDict([
        ('command_id', RadioGroundSubpacketField(b'|h', 1)),
        ('command_data', RadioGroundSubpacketField(b'|h', -1))
    ])),
    cmd_done=RadioGroundSubpacketDefinition(handle_cmd_done, -1, b'DID', OrderedDict([
        ('command_id', RadioGroundSubpacketField(b'|h', 1)),
        ('command_data', RadioGroundSubpacketField(b'|h', -1))
    ])),
    cmd_bad=RadioGroundSubpacketDefinition(handle_cmd_bad, -1, b'BADCMD', OrderedDict([
        ('command_id', RadioGroundSubpacketField(b'|h', 1)),
        ('command_data', RadioGroundSubpacketField(b'|h', -1))
    ])),
    bad_herc=RadioGroundSubpacketDefinition(handle_bad_herc, 15, b'BADHERC', OrderedDict([
        ('issue', RadioGroundSubpacketField(b'|h', 2))
    ])),
    bad_endpoint=RadioGroundSubpacketDefinition(handle_bad_endpoint, 18, b'BADENDP', OrderedDict([
        ('endpoint', RadioGroundSubpacketField(b'|h', 1)),
        ('issue', RadioGroundSubpacketField(b'|h', 2))
    ])),
    bad_udp=RadioGroundSubpacketDefinition(handle_bad_udp, 23, b'BADUDP', OrderedDict([
        ('issue', RadioGroundSubpacketField(b'|h', 1)),
        ('ip', RadioGroundSubpacketField(b'|h', 4)),
        ('port', RadioGroundSubpacketField(b'|h', 2))
    ])),
    critical_reset=RadioGroundSubpacketDefinition(handle_critical_reset, 13, b'CRIT!', OrderedDict([
        ('issue', RadioGroundSubpacketField(b'|h', 2))
    ])),
    critical_reset_swe=RadioGroundSubpacketDefinition(handle_critical_reset_swe, 17, b'SWE!', OrderedDict([
        ('memory_address', RadioGroundSubpacketField(b'|h', 4)),
        ('exception_type', RadioGroundSubpacketField(b'|h', 1))
    ]))
)


class RadioGroundPacketInterface(Packet[CT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class RadioGroundPacket(RadioGroundPacketInterface[RadioGroundPacketInterface]):
    """
    Creates a data-structure to allow storing and handling messages sent
    directly from the Radio's internal MCU.

    @author: Connor W. Colombo (CMU)
    @last-updated: 09/14/2022
    """
    __slots__: List[str] = [
        '_subpacket_name'
    ]

    _subpacket_name: str

    # Public get, private set:
    @property
    def subpacket_name(self) -> str: return self._subpacket_name

    def __init__(self,
                 subpacket_name: Optional[str] = None,
                 payloads: Optional[EnhancedPayloadCollection] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        if subpacket_name is None:
            # Likely we're creating a minimal packet so this information isn't
            # available yet.
            self._subpacket_name = "NO_SUBPACKET_TYPE"
        else:
            self._subpacket_name = subpacket_name

        if raw is None:
            # We're going to enforce that there should always be `raw` data for
            # this packet since this packet always comes from raw data (it's
            # always uplinked) and uses special encodings.
            # Warn if that's not the case:
            logger.warning(
                f"Attempting to build a `RadioGroundPacket` without providing "
                f"`_raw` data. This shouldn't happen. Investigate where "
                f"the raw data got removed."
            )

        if raw is None and (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.warning(
                "A `RadioGroundPacket` was constructed with no `payloads` and "
                "no `raw` data. This suggests it's being created from "
                "nothing as a completely empty packet. Was this "
                "intentional or is this a bug?"
            )

        if payloads is None:
            payloads = EnhancedPayloadCollection()

        super().__init__(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )  # passthru

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> RadioGroundPacket:
        # Check if any RadioGroundSubpacketDefinitions support this data (should be exactly 1):
        supported = [
            (name, sp)
            for (name, sp) in RADIO_GROUND_SUBPACKETS.items()
            if sp.is_valid(data)
        ]

        if len(supported) == 0:
            raise PacketDecodingException(
                data,
                (
                    f"Attempting to parse a `RadioGroundPacket` but no "
                    f"`RadioGroundSubpacketDefinitions` support the given data. "
                    f"Is this actually a `RadioGroundPacket`?"
                )
            )

        # Grab first matching RadioGroundSubpacketDefinitions:
        # There should only exactly one but, if something weird happened and there are multiple, that warning is handled during the `is_valid` check:
        subpacket_name, parser = supported[0]

        # Parse the data into payloads:
        payloads = parser.parse(data)

        return cls(subpacket_name=subpacket_name, payloads=payloads, raw=data, endianness_code=endianness_code)

    def encode(self, **kwargs: Any) -> bytes:
        # Since this packet always comes from raw data (it's always uplinked)
        # and uses special encodings, for simplicity we're just going to
        # enforce that this packet always has to keep a copy of its raw around.
        # So, we're just going to return that.
        # There's no encoding to do. Just pass out the raw data.
        if self._raw is None:
            # We never should be here unless someone has removed the raw.
            # Warn about it:
            logger.warning(
                f"Attempting to encode a `RadioGroundPacket` that has no "
                f"`_raw` data. This shouldn't happen. Did the raw data get "
                f"removed somehow?"
            )
            return b''
        else:
            return self._raw

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: EnhancedPayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> RadioGroundPacket:
        """ Minimum packet is just the packet. """
        return cls(
            # don't worry about `subpacket_name` since it'll come from `__getstate__`
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    def __getstate__(self) -> Dict[str, Any]:
        """Encode metadata which is not stored in `payloads`.
        """
        return {
            **super().__getstate__(),
            'spn': self._subpacket_name
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
        """Retrieve metadata which is not stored in `payloads`."""
        super().__setstate__(data)
        full_dict_spec_check(
            data,
            {
                'spn': str
            },
            name='data'
        )
        self._subpacket_name = data['spn']

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """Valid if the packet starts with `b'DEBUG'`."""
        has_header = (len(data) >= 4 and data[:4] == b'RAD:')
        if not has_header:
            return False
        else:
            # Check if it matches any RadioGroundSubpacketDefinitions
            supported = [
                sp
                for sp in RADIO_GROUND_SUBPACKETS.values()
                if sp.is_valid(data)
            ]
            num_matches = len(supported)
            if num_matches == 0:
                return False
            elif num_matches == 1:
                return True
            else:  # >1
                # Somehow there were multiple matches? This shouldn't be the
                # case. Just parse with the first one.
                logger.warning(
                    f"Multiple RadioGroundSubpacketDefinitions "  # type: ignore
                    f"({supported}) support received packet. Using "
                    f"highest in preference order: {supported[0]}. "
                    f"Packet data: \n{scp.hexdump(data, dump=True)}\n"
                )
                return True

    def __repr__(self) -> str:
        if self._raw is None:
            l = 0
            raw_data = b''
        else:
            l = len(self._raw)
            raw_data = self._raw

        # Grab all telemetry and events:
        telem = [*self.payloads[TelemetryPayload]]
        events = [*self.payloads[EventPayload]]

        msg = (
            f"RadioGroundPacket-{self.subpacket_name}[{l}B]: "
            f"\t{len(telem)}T + {len(events)}E:"
            f"\n{scp.hexdump(raw_data, dump=True)}\n"
        )

        return msg

    def __str__(self) -> str:
        if self._raw is None:
            l = 0
        else:
            l = len(self._raw)

        # Grab all telemetry and events:
        telem = [*self.payloads[TelemetryPayload]]
        events = [*self.payloads[EventPayload]]

        msg = (
            f"RadioGroundPacket-{self.subpacket_name}[{l}B]: "
            f"\t{len(telem)}T + {len(events)}E"
        )

        # If there's exactly one event in here, then this packet
        # directly corresponds with an event and we should print that too:
        if len(events) == 1:
            msg += f" \t{events[0]!s}"

        return msg
