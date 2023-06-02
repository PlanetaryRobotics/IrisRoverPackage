"""
Mixin for GDS packet classes that, themselves, represent events just because we
received them.

This enforces standardized formatting across all such events.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023
"""
from __future__ import annotations

from typing import Final, List, TypeVar, Type, Optional

from IrisBackendv3.data_standards.module import Module, Event

from IrisBackendv3.codec.settings import settings, ENDIANNESS_CODE
from IrisBackendv3.codec.payload import EventPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection
from IrisBackendv3.codec.logs import logger

from IrisBackendv3.codec.packet_classes.packet import Packet, CT

from ansi2html import Ansi2HTMLConverter
_ANSI_CONVERTER = Ansi2HTMLConverter()

MSG_ARGUMENT_NAME: Final[str] = 'msg'

GDS_EVT_PT = TypeVar('GDS_EVT_PT', bound='GdsPacketEventPacket')


def ansi_to_html(msg: str) -> str:
    """Converting any ANSI escape code formatting in message into HTML so it
    can be transmitted."""
    return _ANSI_CONVERTER.convert(
        msg,
        full=False,
        ensure_trailing_newline=False
    )


class GdsPacketEventMixin:
    @staticmethod
    def _PACKET_EVENT_MODULE() -> Module:
        """
        DataStandards module that all such packets-as-events reside in.

        Not storing this as a global and instead leaving it to 'runtime'
        execution so it runs AFTER DataStandards have been loaded (once the
        program is actually running and processing packets).
        """
        return settings['STANDARDS'].modules['GdsPackets']

    @classmethod
    def _PACKET_EVENT_EVENT(cls) -> Event:
        """Not storing this as a global and instead leaving it to 'runtime'
        execution so it runs AFTER DataStandards have been loaded (once the
        program is actually running and processing packets).
        """
        return cls._PACKET_EVENT_MODULE().events[cls.__name__]

    @classmethod
    def append_intrinsic_packet_events(
        cls,
        payloads: EnhancedPayloadCollection,
        messages: List[str],
        rover_timestamp: int = 0
    ) -> EnhancedPayloadCollection:
        """Builds & appends the intrinsic Event(s) that receiving this packet
        class represents to the given Payload collection.

        Optionally a rover-time timestamp can be passed in but, generally, we
        don't know this so it's best to just keep it as `0` (unknown) and rely
        on other stages of the pipeline to timestamp.

        Returns a handle to the payload collection.
        """
        # Get max number of bytes allowed in message:
        maxlen = cls._PACKET_EVENT_EVENT().args[0].datatype.get_max_num_bytes()

        payloads.extend([
            EventPayload(
                module_id=cls._PACKET_EVENT_MODULE().ID,
                event_id=cls._PACKET_EVENT_EVENT().ID,
                args={
                    # Pass along message, converting any ANSI escape code
                    # formatting into HTML so it can be transmitted:
                    MSG_ARGUMENT_NAME: ansi_to_html(msg)[:maxlen]
                },
                timestamp=rover_timestamp,
                raw=None  # **don't** pass the raw since that expects FPrime encoding and the Radio uses its own encoding
            )
            for msg in messages
        ])
        return payloads


class GdsPacketEventPacket(Packet[GDS_EVT_PT], GdsPacketEventMixin):
    """
    Stronger form of `GdsPacketEventMixin` that acts as a base class
    alternative to `Packet[CT]`.

    This is to be used for packets which don't contain payloads inside them
    but, instead, represent one singular event payload.
    """

    def __init__(
        self,
        payloads: Optional[EnhancedPayloadCollection] = None,
        raw: Optional[bytes] = None,
        endianness_code: str = ENDIANNESS_CODE
    ) -> None:
        if raw is None and (payloads is None or payloads.is_empty()):
            # If both `payloads` and `raw` are `None`, what even caused
            # this to be generated?
            logger.debug(
                f"A `GdsPacketEventPacket` ({self.__class__.__name__}) was "
                "constructed with no `payloads` and "
                "and no `raw` data. This suggests it's being created from "
                "nothing as a completely empty packet. Was this "
                "intentional or is this a bug?"
            )

        if payloads is None:
            # Except possibly in future subclasses, this should normally be
            # empty for an `WatchdogDebugPacket`. `payloads` needs to stay as
            # an `__init__` arg to avoid violating the Liskov substitution
            # principle.
            payloads = EnhancedPayloadCollection()

        super().__init__(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )  # passthru

    @classmethod
    def decode(
        cls: Type[GDS_EVT_PT],
        data: bytes,
        endianness_code: str = ENDIANNESS_CODE
    ) -> GDS_EVT_PT:
        # Construct a base version of the packet without payloads:
        base = cls(
            raw=data,
            endianness_code=endianness_code
        )

        # Create payload collection containing intrinsic event(s) and add to
        # the base version:
        # In this case, just use the string representation of the base class
        # as the message:
        base.payloads = cls.append_intrinsic_packet_events(
            EnhancedPayloadCollection(),
            messages=[base.__str__()]
        )

        return base
