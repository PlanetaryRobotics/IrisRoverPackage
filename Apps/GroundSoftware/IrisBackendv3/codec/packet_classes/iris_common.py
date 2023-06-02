"""
Defines `IrisCommonPacket` and `Legacy2020IrisCommonPacket`, implementations of
codec standards for all data sent over DSN for Iris, except data which is
parsed by the Watchdog. `IrisCommonPacket` is the current and
`Legacy2020IrisCommonPacket` represents a deprecated standard which is still
required for backwards compatibility (reading old archived pcaps).

@author: Connor W. Colombo (CMU)
@last-updated: 05/04/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from IrisBackendv3.codec.packet_classes.packet import Packet, CT

from collections import OrderedDict
from typing import List, Any, Optional, cast, Dict
import traceback

import struct
import numpy as np  # type: ignore
import scapy.all as scp  # type: ignore

from IrisBackendv3.codec.magic import Magic, MAGIC_SIZE
from IrisBackendv3.codec.container import ContainerCodec
from IrisBackendv3.codec.payload import TelemetryPayload, EventPayload, FileBlockPayload, CommandPayload
from IrisBackendv3.codec.payload_collection import EnhancedPayloadCollection, extract_downlinked_payloads

from IrisBackendv3.codec.settings import ENDIANNESS_CODE
from IrisBackendv3.codec.logs import logger
from IrisBackendv3.codec.exceptions import PacketDecodingException

from IrisBackendv3.utils.basic import full_dict_spec_check


CPH_SIZE = 4


class IrisCommonPacketInterface(Packet[CT]):
    # empty __slots__ preserves parent class __slots__
    __slots__: List[str] = []


class IrisCommonPacket(IrisCommonPacketInterface[IrisCommonPacketInterface]):
    """
    Defines Common Data Required for All Packets following the Iris Common
    Packet standard.

    Packets follow the standard defined in the IRIS Command & Telemetry List.

    All data is stored in the unpacked form since that's how it will most often
    be handled and transformed and only packed into bytes when needed.

    @author: Connor W. Colombo (CMU)
    @last-updated: 03/07/2022
    """

    # TODO: Do these MTUs matter? Should we take them out? (might matter when
    # sending data just as a sanity / safety check to make sure we don't blow out a buffer?...)
    # [Bytes] - Maximum Transmission Units for Packets thru Watchdog:
    MTU_WATCHDOG: int = 255
    # [Bytes] - Maximum Transmission Units for Packets thru Hercules:
    MTU_HERCULES: int = 1006

    class CommonPacketHeaderInterface(ContainerCodec[CT]):
        # empty __slots__ preserves parent class __slots__
        __slots__: List[str] = []

    class CommonPacketHeader(CommonPacketHeaderInterface[CommonPacketHeaderInterface]):
        """
        Container for Handling and Processing CommonPacketHeaders.

        All data is stored in the unpacked form since that's how it will most
        often be handled and transformed and only packed into bytes when needed.
        """

        SEQ_NUM_SYM: str = 'B'  # struct symbol string for en/decoding seq_num
        VLP_LEN_SYM: str = 'H'  # struct symbol string for en/decoding vlp_len
        # struct symbol string for encoding checksum (externally from int):
        CHECKSUM_SYM: str = 'B'

        __slots__: List[str] = [
            '_seq_num',  # - Sequence Number
            '_vlp_len',  # - Variable Length Payload (VLP) Length
            '_checksum'  # - Checksum of Entire Variable Length Payload (VLP)
        ]

        _seq_num: int
        _vlp_len: int
        _checksum: bytes

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)
        @property
        def seq_num(self) -> int: return self._seq_num
        @property
        def vlp_len(self) -> int: return self._vlp_len
        @property
        def checksum(self) -> bytes: return self._checksum

        def __init__(self,
                     seq_num: int,
                     vlp_len: int,
                     checksum: Optional[bytes] = None,
                     raw: Optional[bytes] = None,
                     endianness_code: str = ENDIANNESS_CODE
                     ) -> None:
            self._seq_num = seq_num
            self._vlp_len = vlp_len

            if checksum is None:
                # init as all zeros (before it's computed later):
                self._checksum = struct.pack(
                    endianness_code + IrisCommonPacket.CommonPacketHeader.CHECKSUM_SYM, 0x00)
            else:
                self._checksum = checksum

            super().__init__(raw=raw, endianness_code=endianness_code)

        def __str__(self) -> str:
            return f"CPH[{self.seq_num}]: {self.vlp_len}B"

        @classmethod
        def decode(cls,
                   data: bytes,
                   endianness_code: str = ENDIANNESS_CODE
                   ) -> IrisCommonPacket.CommonPacketHeader:
            """Extract all data in the given raw common packet header."""
            cph_head, checksum = data[:-1], data[-1:]

            sns = IrisCommonPacket.CommonPacketHeader.SEQ_NUM_SYM
            vls = IrisCommonPacket.CommonPacketHeader.VLP_LEN_SYM
            # TODO: What's up with the `''` here? - just a relic from a misunderstanding or a typo? It works, so likely just it was thought to be necessary.
            struct_str = endianness_code + ' ' + sns + '' + vls
            seq_num, vlp_len = struct.unpack(struct_str, cph_head)

            return cls(
                endianness_code=endianness_code,
                seq_num=seq_num,
                vlp_len=vlp_len,
                checksum=checksum,
                raw=data
            )

        def encode(self, **kwargs: Any) -> bytes:
            """Pack data into a bytes object."""

            sns = IrisCommonPacket.CommonPacketHeader.SEQ_NUM_SYM
            vls = IrisCommonPacket.CommonPacketHeader.VLP_LEN_SYM
            struct_str = self.endianness_code + ' ' + sns + '' + vls
            cph_head = struct.pack(struct_str, self.seq_num, self.vlp_len)

            self._raw = cph_head + self.checksum

            return self._raw

    __slots__: List[str] = [
        # CommonPacketHeader Data:
        '_common_packet_header'
    ]

    _common_packet_header: IrisCommonPacket.CommonPacketHeader

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def common_packet_header(self) -> IrisCommonPacket.CommonPacketHeader:
        return self._common_packet_header

    @staticmethod
    def _count_vlp_len(payloads: EnhancedPayloadCollection) -> int:
        """
        Calculates the VLP length necessary to send the given PayloadsCollection.
        Sum of all Payload sizes + MAGIC_SIZE * num_payloads
        """
        total_size = 0
        num_payloads = 0

        for p in payloads.all_payloads:
            total_size += len(p.encode())
            num_payloads += 1

        total_size += num_payloads * MAGIC_SIZE  # Each will require a magic
        return total_size

    def __str__(self) -> str:
        # If this contains only commands just print the command(s):
        cmds = [*self.payloads[CommandPayload]]
        if len(cmds) == len(self.payloads):
            return (
                f"ICP["
                f"#{self.common_packet_header.seq_num}::"
                f"{self.common_packet_header.vlp_len}] > "
                f"{', '.join(str(c) for c in cmds)}"
            )

        # Grab the string of the latest value for each unique telemetry channel:
        # dict of module name -> channel_name -> payload string
        latest: OrderedDict[str, OrderedDict[str, Any]] = OrderedDict()
        for payload in self.payloads[TelemetryPayload]:
            payload = cast(TelemetryPayload, payload)
            if payload.module.name not in latest:
                latest[payload.module.name] = OrderedDict()  # init
            latest[payload.module.name][payload.channel.name] = str(payload)

        # Sort by module name (make it easier to read):
        latest = OrderedDict(sorted(latest.items(), key=lambda kv: kv[0]))

        # Sort the channels in each module list by channel name (make it easier to read):
        for module_name in latest.keys():
            latest[module_name] = OrderedDict(
                sorted(latest[module_name].items(), key=lambda kv: kv[0])
            )

        # Append the latest telemetry strings with a new line for each module:
        out = self.__repr__() + '\n > Latest Telemetry: '
        for module_latest in latest.values():
            out += '\n' + ',\t '.join([f'{p}' for p in module_latest.values()])

        return out

    def __repr__(self) -> str:
        return (
            f"ICP["
            f"#{self.common_packet_header.seq_num}::"
            f"{self.common_packet_header.vlp_len}]: "
            f"\t{len(list(self.payloads[TelemetryPayload]))} T"
            f"\t- {len(list(self.payloads[EventPayload]))} E"
            f"\t- {len(list(self.payloads[FileBlockPayload]))} B"
            f"\t- {len(list(self.payloads[CommandPayload]))} C"
        )

    def __init__(self,
                 payloads: EnhancedPayloadCollection,
                 seq_num: int = 0,
                 common_packet_header: Optional[CommonPacketHeader] = None,
                 ignore_state_members: bool = False,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if ignore_state_members:
            # In this case, don't worry about how you init any member variables
            # that are part of `__getstate__`, so just fill with empty data
            # since it will likely be replaced immediately.
            # Designed for use by `build_minimum_packet`.
            #
            # NOTE: This is to be used when reconstructing data from
            # `__reduce__` b/c if this `Packet` was been encoded by `__reduce__`
            # (e.g. for IPC), `__init__` gets called then the state gets
            # populated from `__getstate__`. So, this `__init__` will be called
            # w/o `common_packet_header` when decoding data from `__reduce__`
            # but the `common_packet_header` and `seq_num` args won't matter b/c
            # it will just get overwritten by whatever was in`__getstate__`.
            self._common_packet_header = IrisCommonPacket.CommonPacketHeader(
                seq_num=seq_num,
                vlp_len=1  # arbitrary length >0
            )

        else:
            # any data given or not given here was intentional and so any
            # absences need to be computed:
            if common_packet_header is None:
                self._common_packet_header = IrisCommonPacket.CommonPacketHeader(
                    seq_num=seq_num,
                    vlp_len=IrisCommonPacket._count_vlp_len(payloads)
                )
            else:
                self._common_packet_header = common_packet_header

        super().__init__(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: EnhancedPayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> IrisCommonPacket:
        """ Builds a minimum representation of this Packet (before any
        additional elements from `__getstate__` are added back). Used by
        `Packet.__reduce__` for unpacking serialized data.
        """
        return cls(
            # don't worry about `seq_num` and `common_packet_header` since they'll come from `__getstate__`
            ignore_state_members=True,
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    def __getstate__(self) -> Dict[str, Any]:
        """Encode metadata which is not stored in `payloads`.
        """
        return {
            'cph': self._common_packet_header
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
        """Retrieve metadata which is not stored in `payloads`."""
        full_dict_spec_check(
            data,
            {
                'cph': IrisCommonPacket.CommonPacketHeader
            },
            name='data'
        )

        self._common_packet_header = data['cph']

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> IrisCommonPacket:
        """Construct a Iris Packet Object from Bytes.
            NOTE: This does not add metadata to the Payloads like `Pathway`,
            `Source`, or `DownlinkTimes`. That needs to be added to the
            Payloads at the `Transceiver` layer.
        """

        cph_data = data[:CPH_SIZE]
        CPH = IrisCommonPacket.CommonPacketHeader.decode(
            cph_data,
            endianness_code=endianness_code
        )
        #! TODO: Perform checksum check. (not impl. in FSW atm)
        actual_vlp_len = len(data) - CPH_SIZE
        if CPH.vlp_len != actual_vlp_len:
            raise PacketDecodingException(
                cph_data,
                (
                    f"CPH {CPH} was successfully decoded *but* the length it "
                    f"expects for the VLP({CPH.vlp_len}B) doesn't match the "
                    f"actual VLP length ({actual_vlp_len}B)."
                )
            )
        # Extract the Variable Length Payload
        VLP = data[CPH_SIZE:]

        # Parse VLP:
        try:
            payloads = extract_downlinked_payloads(
                VLP=VLP,
                endianness_code=endianness_code
            )

        except Exception as e:
            trace = traceback.format_exc()
            logger.warning(
                f"Had to abort packet parsing due to the following exception: `{trace}`. "
                f"The data being parsed was: \n"
                f"{scp.hexdump(data, dump=True)}\n"
            )
            payloads = EnhancedPayloadCollection()

        return IrisCommonPacket(
            common_packet_header=CPH,
            payloads=payloads,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        # Compile all payloads:
        VLP = b''
        for payload in self.payloads.all_payloads:
            VLP += payload.magic.encode() + payload.encode()

        CPH = self.common_packet_header.encode()
        assert self.common_packet_header.vlp_len == len(VLP)

        packet = CPH + VLP

        # Compute Checksum over entire packet (with checksum defaulted to 0):
        self.common_packet_header._checksum = struct.pack(
            self.endianness_code + IrisCommonPacket.CommonPacketHeader.CHECKSUM_SYM,
            ~np.uint8(sum(bytearray(packet)) % 256)
        )

        # Rebuild CPH with checksum:
        CPH = self.common_packet_header.encode()

        # Rebuild Packet with checksum:
        packet = CPH + VLP

        return packet

    @ classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        # CPH + 4B Magic + at least 1B of data:
        min_length = len(data) > (CPH_SIZE + MAGIC_SIZE + 1)
        try:
            magic = Magic.decode(
                data[CPH_SIZE:CPH_SIZE+MAGIC_SIZE],
                byte_order=endianness_code
            )
            contains_magic = magic != Magic.MISSING
        except (PacketDecodingException, IndexError):
            contains_magic = False

        return min_length and contains_magic


class Legacy2020IrisCommonPacket(IrisCommonPacketInterface[IrisCommonPacketInterface]):
    """
    Legacy Version of `IrisCommonPacket` to support parsing old FSW pcaps from
    2020 and early 2021 which use a 2B checksum (which often wasn't populated).

    @author: Connor W. Colombo (CMU)
    @last-updated: 10/02/2021
    """

    LEGACY2020_CPH_SIZE: int = 5

    class Legacy2020CommonPacketHeaderInterface(ContainerCodec[CT]):
        # empty __slots__ preserves parent class __slots__
        __slots__: List[str] = []

    class Legacy2020CommonPacketHeader(Legacy2020CommonPacketHeaderInterface[Legacy2020CommonPacketHeaderInterface]):
        """
        Legacy Version of `CommonPacketHeader` to support parsing old FSW pcaps from
        2020 and early 2021 which use a 2B checksum (which often wasn't populated)
        and certain standards were adhered to more laxly.

        SHOULD NOT MAKE IT INTO MISSION SOFTWARE.
        """

        SEQ_NUM_SYM: str = 'B'  # struct symbol string for en/decoding seq_num
        VLP_LEN_SYM: str = 'H'  # struct symbol string for en/decoding vlp_len
        # struct symbol string for encoding checksum (externally from int):
        CHECKSUM_SYM: str = 'H'

        __slots__: List[str] = [
            '_seq_num',  # - Sequence Number
            '_vlp_len',  # - Variable Length Payload (VLP) Length
            '_checksum'  # - Checksum of Entire Variable Length Payload (VLP)
        ]

        _seq_num: int
        _vlp_len: int
        _checksum: bytes

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)
        @property
        def seq_num(self) -> int: return self._seq_num
        @property
        def vlp_len(self) -> int: return self._vlp_len
        @property
        def checksum(self) -> bytes: return self._checksum

        def __init__(self,
                     seq_num: int,
                     vlp_len: int,
                     checksum: Optional[bytes] = None,
                     raw: Optional[bytes] = None,
                     endianness_code: str = ENDIANNESS_CODE
                     ) -> None:
            self._seq_num = seq_num
            self._vlp_len = vlp_len

            if checksum is None:
                # init as all zeros (before it's computed later):
                self._checksum = struct.pack(
                    endianness_code + IrisCommonPacket.CommonPacketHeader.CHECKSUM_SYM, 0x0000)
            else:
                self._checksum = checksum

            super().__init__(raw=raw, endianness_code=endianness_code)

        def __str__(self) -> str:
            return f"CPH[{self.seq_num}]: {self.vlp_len}B"

        @classmethod
        def decode(cls,
                   data: bytes,
                   endianness_code: str = ENDIANNESS_CODE
                   ) -> Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader:
            """Extract all data in the given raw common packet header."""
            cph_head, checksum = data[:-2], data[-2:]

            sns = Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader.SEQ_NUM_SYM
            vls = Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader.VLP_LEN_SYM
            struct_str = endianness_code + ' ' + sns + '' + vls
            seq_num, vlp_len = struct.unpack(struct_str, cph_head)

            return cls(
                endianness_code=endianness_code,
                seq_num=seq_num,
                vlp_len=vlp_len,
                checksum=checksum,
                raw=data
            )

        def encode(self, **kwargs: Any) -> bytes:
            """Pack data into a bytes object."""

            sns = Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader.SEQ_NUM_SYM
            vls = Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader.VLP_LEN_SYM
            struct_str = self.endianness_code + ' ' + sns + '' + vls
            cph_head = struct.pack(struct_str, self.seq_num, self.vlp_len)

            self._raw = cph_head + self.checksum

            return self._raw

    __slots__: List[str] = [
        # CommonPacketHeader Data:
        '_common_packet_header'
    ]

    _common_packet_header: Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader

    # Make public get, private set to signal that you can freely use these values
    # but modifying them directly can yield undefined behavior (specifically
    # `raw` not syncing up with whatever other data is in the container)
    @property
    def common_packet_header(self) -> Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader:
        return self._common_packet_header

    @staticmethod
    def _count_vlp_len(payloads: EnhancedPayloadCollection) -> int:
        """
        Calculates the VLP length necessary to send the given PayloadsCollection.
        Sum of all Payload sizes + MAGIC_SIZE * num_payloads
        """
        total_size = 0
        num_payloads = 0

        for p in payloads.all_payloads:
            total_size += len(p.encode())
            num_payloads += 1

        total_size += num_payloads * MAGIC_SIZE  # Each will require a magic
        return total_size

    def __str__(self) -> str:
        # If this contains only commands just print the command(s):
        cmds = [*self.payloads[CommandPayload]]
        if len(cmds) == len(self.payloads):
            return (
                f"LEGACY2020 ICP["
                f"#{self.common_packet_header.seq_num}::"
                f"{self.common_packet_header.vlp_len}] > "
                f"{', '.join(str(c) for c in cmds)}"
            )

        # Otherwise, just summarize the telemetry:
        base = self.__repr__()

        # Grab the string of the latest value for each unique telemetry channel:
        latest = {}
        for payload in self.payloads[TelemetryPayload]:
            payload = cast(TelemetryPayload, payload)
            latest[(payload.module_id, payload.channel_id)] = str(payload)

        # Append the latest telemetry strings:
        out = (
            base +
            '\n\t Latest Telemetry: ' +
            ',\t '.join([f'{p}' for p in latest.values()])
        )

        return out

    def __repr__(self) -> str:
        return (
            f"LEGACY2020 ICP["
            f"#{self.common_packet_header.seq_num}::"
            f"{self.common_packet_header.vlp_len}]: "
            f"\t{len(list(self.payloads.TelemetryPayload))} T"  # Telemetry
            f"\t- {len(list(self.payloads.EventPayload))} E"
            f"\t- {len(list(self.payloads.FileBlockPayload))} B"  # File Blocks
            f"\t- {len(list(self.payloads.CommandPayload))} C"
        )

    def __init__(self,
                 payloads: EnhancedPayloadCollection,
                 seq_num: int = 0,
                 common_packet_header: Optional[Legacy2020CommonPacketHeader] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if common_packet_header is None:
            self._common_packet_header = Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader(
                seq_num=seq_num,
                vlp_len=Legacy2020IrisCommonPacket._count_vlp_len(payloads)
            )
        else:
            self._common_packet_header = common_packet_header
        super().__init__(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    @classmethod
    def build_minimum_packet(
        cls,
        payloads: EnhancedPayloadCollection,
        raw: Optional[bytes],
        endianness_code: str
    ) -> Legacy2020IrisCommonPacket:
        """ Builds a minimum representation of this Packet (before any
        additional elements from `__getstate__` are added back). Used by
        `Packet.__reduce__` for unpacking serialized data.
        """
        return cls(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )

    def __getstate__(self) -> Dict[str, Any]:
        """Encode metadata which is not stored in `payloads`.
        """
        return {
            **super().__getstate__(),
            'cph': self._common_packet_header
        }

    def __setstate__(self, data: Dict[str, Any]) -> None:
        """Retrieve metadata which is not stored in `payloads`."""
        super().__setstate__(data)
        full_dict_spec_check(
            data,
            {
                'cph': IrisCommonPacket.CommonPacketHeader
            },
            name='data'
        )

        self._common_packet_header = data['cph']

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE
               ) -> Legacy2020IrisCommonPacket:
        """Construct a Iris Packet Object from Bytes."""

        cph_data = data[:Legacy2020IrisCommonPacket.LEGACY2020_CPH_SIZE]
        CPH = Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader.decode(
            cph_data,
            endianness_code=endianness_code
        )
        # NOTE: Ignore checksum check for Legacy2020. Wasn't impl. yet in FSW.
        actual_vlp_len = (
            len(data) - Legacy2020IrisCommonPacket.LEGACY2020_CPH_SIZE
        )
        if CPH.vlp_len != actual_vlp_len:
            raise PacketDecodingException(
                cph_data,
                (
                    f"CPH {CPH} was successfully decoded *but* the length it "
                    f"expects for the VLP({CPH.vlp_len}B) doesn't match the "
                    f"actual VLP length ({actual_vlp_len}B)."
                )
            )
        # Extract the Variable Length Payload
        VLP = data[Legacy2020IrisCommonPacket.LEGACY2020_CPH_SIZE:]

        # Parse VLP:
        try:
            payloads = extract_downlinked_payloads(
                VLP=VLP,
                endianness_code=endianness_code
            )

        except Exception as e:
            trace = traceback.format_exc()
            logger.warning(
                f"Had to abort packet parsing due to the following exception: `{trace}`. "
                f"The data being parsed was: \n"
                f"{scp.hexdump(data, dump=True)}\n"
            )
            payloads = EnhancedPayloadCollection()

        return Legacy2020IrisCommonPacket(
            common_packet_header=CPH,
            payloads=payloads,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        # Compile all payloads:
        VLP = b''
        for payload in self.payloads.all_payloads:
            VLP += payload.magic.encode() + payload.encode()

        CPH = self.common_packet_header.encode()
        assert self.common_packet_header.vlp_len == len(VLP)

        packet = CPH + VLP

        # Compute Checksum over entire packet (with checksum defaulted to 0):
        self.common_packet_header._checksum = struct.pack(
            self.endianness_code +
            Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader.CHECKSUM_SYM,
            ~np.uint8(sum(bytearray(packet)) % 256)
        )

        # Rebuild CPH with checksum:
        CPH = self.common_packet_header.encode()

        # Rebuild Packet with checksum:
        packet = CPH + VLP

        return packet

    @ classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        # CPH + 4B Magic + at least 1B of data:
        min_length = len(data) > (
            Legacy2020IrisCommonPacket.LEGACY2020_CPH_SIZE + MAGIC_SIZE + 1
        )
        try:
            magic = Magic.decode(
                data[Legacy2020IrisCommonPacket.LEGACY2020_CPH_SIZE:
                     Legacy2020IrisCommonPacket.LEGACY2020_CPH_SIZE+MAGIC_SIZE],
                byte_order=endianness_code
            )
            contains_magic = magic != Magic.MISSING
        except (PacketDecodingException, IndexError):
            contains_magic = False

        return min_length and contains_magic
