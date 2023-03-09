"""
Defines `RadioHelloPacket`, a `Packet` wrapper for a hello message from the
Radio, emitted whenever the Radio (re)connects to a network. This packet type
doesn't contain any telemetry but does contain a single EventPayload and is
just printed to the console.

@author: Connor W. Colombo (CMU)
@last-updated: 09/14/2022
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations

from IrisBackendv3.codec.packet_classes.packet import Packet, CT

from typing import Any, Optional, Final, List, cast

from scapy.utils import hexdump  # type: ignore

from IrisBackendv3.codec.payload import EventPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.fsw_data_codec import StringPacker

from IrisBackendv3.codec.settings import ENDIANNESS_CODE, settings
from IrisBackendv3.codec.logging import logger


class RadioHelloPacketInterface(Packet[CT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class RadioHelloPacket(RadioHelloPacketInterface[RadioHelloPacketInterface]):
    """
    Creates a data-structure to allow storing and handling a debug string from
    the Radio.

    @author: Connor W. Colombo (CMU)
    @last-updated: 09/14/2022
    """
    __slots__: List[str] = []  # empty __slots__ preserves parent __slots__

    # Core message that we expect to see:
    EXPECTED_MESSAGE: Final[bytes] = b'Hello Earth, this is Iris on the Moon!'

    # This maps to a single Event. These are the specifiers:
    MODULE_NAME: Final[str] = 'RadioGround'
    EVENT_NAME: Final[str] = 'Connected'

    def __init__(self,
                 payloads: Optional[EnhancedPayloadCollection] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if raw is None and (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.debug(
                "A `RadioHelloPacket` was constructed with no `payloads` and "
                "no `raw` data. This suggests it's being created from "
                "nothing as a completely empty packet. Was this "
                "intentional or is this a bug?"
            )

        if payloads is None:
            # Except possibly in future subclasses, this should normally be
            # empty for an `WatchdogHelloPacket`. `payloads` needs to stay as
            # an `__init__` arg to avoid violating the Liskov substitution
            # principle.
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
               ) -> RadioHelloPacket:
        # Grab the corresponding module & event:
        module = settings['STANDARDS'].modules[cls.MODULE_NAME]
        event = module.events[cls.EVENT_NAME]
        # Build the EventPayload:
        payloads = EnhancedPayloadCollection(
            EventPayload=[
                EventPayload(
                    module_id=module.ID,
                    event_id=event.ID,
                    args=dict(
                        message=StringPacker.decode_str(data)
                    ),
                    timestamp=0,  # No timestamp information given.
                    raw=data,
                    endianness_code=endianness_code
                )
            ]
        )
        return cls(raw=data, payloads=payloads, endianness_code=endianness_code)

    def grab_event_payload(self) -> Optional[EventPayload]:
        """Grabs the first EventPayload that matches MODULE_NAME::EVENT_NAME.
        There should be exactly one of these, no more, no fewer, so long as 
        no one's messed with this packet. Still, for robustness, we have to
        grab this properly in case that assumption doesn't hold."""
        # Grab the Module and Event:
        module = settings['STANDARDS'].modules[self.MODULE_NAME]
        event = module.events[self.EVENT_NAME]
        # Grab all EventPayloads:
        eps = cast(List[EventPayload], [*self.payloads[EventPayload]])
        # Grab all RadioGround_Connected EventPayloads:
        eps = [ep for ep in eps
               if ep.module_id == module.ID and ep.event_id == event.ID]
        if len(eps) > 0:
            return eps[0]
        else:
            return None

    def encode(self, **kwargs: Any) -> bytes:
        # There's no encoding to do. It's just raw data.
        if self._raw is None:
            # Attempt to grab the string from the Event and encode it:
            event = self.grab_event_payload()
            if event is not None:
                return StringPacker.encode_str(event.args['message'])
            else:
                # We have nothing (correct) to encode:
                return b''
        else:
            # As long as no one's messed with this packet, this should be all
            # we have to do:
            return self._raw

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: EnhancedPayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> RadioHelloPacket:
        """ Minimum packet is just the packet. """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """Valid if the packet starts with the expected message.
        This still leaves some room in case extra garbage somehow got tacked
        onto the end."""
        return data[:len(cls.EXPECTED_MESSAGE)] == cls.EXPECTED_MESSAGE

    def __repr__(self) -> str:
        # Use the repr from the Event (as long as it's actually there):
        event = self.grab_event_payload()
        if event is not None:
            l = len(event.raw) if event.raw is not None else 0
            data = f"{event!r}"
        elif self._raw is not None:
            l = len(self._raw)
            data = f"{self._raw!r}"
        else:
            # Not really sure how we ended up here but we need to handle this case just in case.
            l = 0
            data = ""
        return f"RadioHelloPacket[{l}B]: {data}"

    def __str__(self) -> str:
        # Use the str from the Event (as long as it's actually there):
        event = self.grab_event_payload()
        if event is not None:
            l = len(event.raw) if event.raw is not None else 0
            data = f"{event!s}"
        elif self._raw is not None:
            l = len(self._raw)
            data = f"{self._raw!r}"
        else:
            # Not really sure how we ended up here but we need to handle this case just in case.
            l = 0
            data = ""
        return f"RadioHelloPacket[{l}B]: {data}"
