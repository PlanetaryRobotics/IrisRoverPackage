"""
Implementation of Watchdog Command Response Packet and its Interface.
This is used as a dense and minimally sized way to communicate the most
important Watchdog statuses during cis-lunar transit.

@author: Connor W. Colombo (CMU)
@last-updated: 06/01/2023
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from .gds_packet_event_mixin import GdsPacketEventMixin

from IrisBackendv3.codec.packet_classes.packet import CT
from IrisBackendv3.codec.packet_classes.custom_payload import CustomPayloadPacket, CPCT

from typing import List, Any, Optional, ClassVar, cast, Union, Type

import struct

from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection

from IrisBackendv3.codec.settings import ENDIANNESS_CODE, settings
from IrisBackendv3.codec.exceptions import PacketDecodingException

from IrisBackendv3.data_standards.module import Module


class WatchdogCommandResponsePacketInterface(CustomPayloadPacket[CT, CPCT]):
    # Name of the corresponding prebuilt `Module` used for mapping this packet's
    # data to telemetry streams:
    PREBUILT_MODULE_NAME: ClassVar[str] = 'WatchdogCommandResponse'

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    class CustomPayload():
        """
        Core custom WatchdogCommandResponse payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogCommandResponse` prebuilt module.
        Note: an error will get thrown by `WatchdogCommandResponsePacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogCommandResponse` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.

        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet. [SEE THE NOTE IN `CustomPayloadPacket` FOR MORE
        DETAILS]
        """

        __slots__: List[str] = [
            '_CommandId',
            '_ErrorFlag'
        ]

        _CommandId: int
        _ErrorFlag: int

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)
        @property
        def CommandId(self) -> int: return self._CommandId
        @property
        def ErrorFlag(self) -> int: return self._ErrorFlag

        @property
        def CommandName(self) -> str:
            """
            Look up the Command Name corresponding to the Command ID given in 
            this packet in the *current ground software* copy of the 
            datastandards (that is, it's not a 100% guarantee that this is how 
            the WD code was interpreting the command ID - *but* appropriate 
            systems controls and version alignment should prevent them from 
            being out of sync).
            """
            # Grab module ID
            mid = settings['STANDARDS'].modules['WatchDogInterface'].ID
            opcode = mid | self.CommandId
            _, command = settings['STANDARDS'].global_command_lookup(opcode)
            return command.name

        @property
        def ErrorFlagName(self) -> str:
            return cast(str, self._module.telemetry['ErrorFlag'].get_enum_name(
                self.ErrorFlag
            ))

        @property
        def ErrorFlagComment(self) -> str:
            item = self._module.telemetry['ErrorFlag'].get_enum_item(
                self.ErrorFlag
            )
            if item is None:
                return ""
            else:
                return item.comment

        @property
        def _module(self) -> Module:
            return WatchdogCommandResponsePacketInterface.get_ds_module()

        def __init__(self,
                     CommandId: int,
                     ErrorFlag: Union[int, str]
                     ) -> None:
            self._CommandId = CommandId

            if isinstance(ErrorFlag, int):
                self._ErrorFlag = ErrorFlag
            else:
                self._ErrorFlag = cast(int, self._module.telemetry['ErrorFlag'].get_enum_value(
                    cast(str, ErrorFlag)
                ))

        def __str__(self) -> str:
            return (
                f"> Command #{self._CommandId} ({self.CommandName}) responded with {self.ErrorFlagName}[{hex(self.ErrorFlag)}]: '{self.ErrorFlagComment}'."
            )

        def __repr__(self) -> str:
            return (
                f"> Command[#{self._CommandId}: {self.CommandName}] -> {self.ErrorFlagName}[{hex(self.ErrorFlag)}]"
            )


# Some useful type alias that make the subsequent class more readable:
WCR_PI = WatchdogCommandResponsePacketInterface
WCR_CP = WCR_PI.CustomPayload


class WatchdogCommandResponsePacket(WCR_PI[WCR_PI, WCR_CP], GdsPacketEventMixin):
    START_FLAG: bytes = b'\x0A'  # Required start flag
    # Specify the `CUSTOM_PAYLOAD_CLASS` used by `CustomPayloadPacket` superclass (weird type signature is used to match the `Optional[Type[CPCT]]` used in the superclass):
    CUSTOM_PAYLOAD_CLASS: Optional[Type[WCR_CP]] = WCR_CP

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    def __repr__(self) -> str:
        return self.custom_payload.__repr__()

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> WatchdogCommandResponsePacket:
        flag, core_data = data[:1], data[1:]
        if cls.START_FLAG != flag:
            raise PacketDecodingException(
                data,
                "Start flag for `WatchdogCommandResponsePacket` was invalid. "  # type: ignore
                f"Expected {cls.START_FLAG}, Got: {flag} ."
            )
        custom_payload = WatchdogCommandResponsePacket.CustomPayload(
            *struct.unpack(endianness_code + '2B', core_data)
        )

        # Create base packet:
        base = WatchdogCommandResponsePacket(
            custom_payload=custom_payload,
            raw=data,
            endianness_code=endianness_code
        )

        # Append intrinsic packet event to the payload collection (currently,
        # just a string of representation of the base packet):
        cls.append_intrinsic_packet_events(
            base.payloads,
            messages=[base.__str__()]
        )

        return base

    def encode(self, **kwargs: Any) -> bytes:
        # TODO (not really a typical use case so not super necessary besides for completeness)
        raise NotImplementedError()

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[:1] == cls.START_FLAG
        right_length = len(data) == 3  # Bytes

        return right_start and right_length

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: EnhancedPayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> WatchdogCommandResponsePacket:
        """ Builds a minimum representation of this Packet (before any
        additional elements from `__getstate__` are added back). Used by
        `Packet.__reduce__` for unpacking serialized data.
        """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )
