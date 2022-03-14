#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Defines Common Data Required for Packets. Support for Building and Parsing
Packets.

@author: Connor W. Colombo (CMU)
@last-updated: 11/21/2021
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List, Any, Optional, TypeVar, cast, Union, Type, Dict
from collections import OrderedDict
from abc import ABC, abstractmethod
from enum import Enum
import traceback

import struct
import bitstruct  # type: ignore
import numpy as np  # type: ignore
import time
from math import ceil
from scapy.utils import hexstr  # type: ignore
from pandas import DataFrame  # type: ignore

from .magic import Magic, MAGIC_SIZE
from .metadata import DataPathway, DataSource
from .container import ContainerCodec
from .payload import PayloadCollection, TelemetryPayload, extract_downlinked_payloads

from .settings import ENDIANNESS_CODE, settings
from .logging import logger
from .exceptions import PacketDecodingException

from IrisBackendv3.utils.basic import flip_all_bits_in_bytes
from IrisBackendv3.data_standards.module import Module

CPH_SIZE = 4

CT = TypeVar('CT')

# TODO: Add update hooks?

# TODO: Add `__str__` / `__repr__`s

#! TODO: Handle serialization (must replace container scheme, augment by storing payloads with their metadata)
#! ^- or don't serialize as packets?... no, must be able to serialize to send over IPC network.


def parse_packet(
    packet_bytes: bytes,
    codecs: Optional[List[Type[Packet]]] = None
) -> Optional[Packet]:
    """
    Parses the given packet bytes (with any network headers already stripped off)
    and returns a packet if the given bytes fit any of the given packet `codecs`
    (Packet classes).
    """
    # Default: All available packet codecs (in order of use preference):
    if codecs is None:
        codecs = [
            WatchdogHeartbeatPacket,
            WatchdogCommandResponsePacket,
            IrisCommonPacket,
            WatchdogTvacHeartbeatPacket,
            Legacy2020IrisCommonPacket
        ]

    # Codecs which support this packet:
    supported = [c for c in codecs if c.is_valid(packet_bytes)]

    # Check for issues:
    if len(supported) == 0:
        if packet_bytes.startswith('DEBUG'.encode('utf-8')):
            debug_msg = packet_bytes[5:].decode('utf-8')
            print(debug_msg, end='')
            return None
        else:
            CodecLogger.warning(
                f"Invalid packet detected. Does not conform to any supported specs: "  # type: ignore
                f"{packet_bytes}"
            )

    if len(supported) > 1:
        logger.warning(
            f"Multiple codecs "  # type: ignore
            f"({supported}) support received packet. Using "
            f"highest in preference order: {supported[0]}. "
            f"Packet data: {packet_bytes} ."
        )

    # Parse Packet:
    packet: Optional[Packet] = None  # default
    try:
        if len(supported) > 0:
            # Parse:
            packet = supported[0].decode(
                packet_bytes,
                pathway=DataPathway.WIRELESS,
                source=DataSource.PCAP
            )
            # Store:
            # ! TODO: Don't need this? This came over from `trans_tools`. Do we need storage on decode here? This should be handled by storage IPC node.
            # packet = cast(Packet, packet)
            # for i in range(len(packet.payloads)):
            #     all_payloads[i].extend(packet.payloads[i])  # type: ignore

    except Exception as e:
        err = e
        trace = traceback.format_exc()
        logger.warning(
            f"Had to abort packet parsing due to the following exception: {err}"
        )
        # Add more information if desired:
        logger.verbose(  # type: ignore
            f"\t > Had to abort packet parsing due to the following exception: {err} from {trace}"
        )

    return packet


class Packet(ContainerCodec[CT], ABC):
    # Mainly an aliasing class for now (allows for creating List[Packet])

    __slots__: List[str] = [
        'pathway',  # DataPathway through which this data was received or should be sent
        'source',  # DataSource of this data (how it entered the GSW)
        'payloads'  # PayloadCollection of all Payloads, separated by type
    ]  # empty but lets the slots from parent continue

    pathway: DataPathway
    source: DataSource
    payloads: PayloadCollection

    def __init__(self,
                 payloads: PayloadCollection,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        self.payloads = payloads
        self.pathway = pathway
        self.source = source
        super().__init__(raw=raw, endianness_code=endianness_code)  # passthru

    @classmethod
    @abstractmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE
               ) -> CT:
        raise NotImplementedError()

    @classmethod
    @abstractmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        raise NotImplementedError()


class IrisCommonPacketInterface(Packet[CT]):
    pass


class IrisCommonPacket(IrisCommonPacketInterface[IrisCommonPacketInterface]):
    """
    Defines Common Data Required for All Packets.

    Packets follow the standard defined in the IRIS Command & Telemetry List.

    All data is stored in the unpacked form since that's how it will most often
    be handled and transformed and only packed into bytes when needed.

    @author: Connor W. Colombo (CMU)
    @last-updated: 04/09/2020
    """

    # TODO: Do these MTUs matter? Should we take them out? (might matter when
    # sending data just as a sanity / safety check to make sure we don't blow out a buffer?...)
    # [Bytes] - Maximum Transmission Units for Packets thru Watchdog:
    MTU_WATCHDOG: int = 255
    # [Bytes] - Maximum Transmission Units for Packets thru Hercules:
    MTU_HERCULES: int = 1006

    class CommonPacketHeaderInterface(ContainerCodec[CT]):
        pass

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
    def _count_vlp_len(payloads: PayloadCollection) -> int:
        """
        Calculates the VLP length necessary to send the given PayloadsCollection.
        Sum of all Payload sizes + MAGIC_SIZE * num_payloads
        """
        total_size = 0
        num_payloads = 0
        for c in payloads:
            c = cast(List, c)  # mypy doesn't get this by itself
            for p in c:
                total_size += len(p.encode())
                num_payloads += 1
        total_size += num_payloads * MAGIC_SIZE  # Each will require a magic
        return total_size

    def __str__(self) -> str:
        base = self.__repr__()

        # Grab the string of the lastest value for each unique telemetry channel:
        latest = {}
        for payload in self.payloads.TelemetryPayload:
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
            f"ICP["
            f"#{self.common_packet_header.seq_num}::"
            f"{self.common_packet_header.vlp_len}]: "
            f"\t{len(self.payloads.TelemetryPayload)} T"  # Telemetry
            f"\t- {len(self.payloads.EventPayload)} E"
            f"\t- {len(self.payloads.FileBlockPayload)} B"  # File Blocks
            f"\t- {len(self.payloads.CommandPayload)} C"
        )

    def __init__(self,
                 payloads: PayloadCollection,
                 seq_num: int = 0,
                 common_packet_header: Optional[CommonPacketHeader] = None,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        if common_packet_header is None:
            self._common_packet_header = IrisCommonPacket.CommonPacketHeader(
                seq_num=seq_num,
                vlp_len=IrisCommonPacket._count_vlp_len(payloads)
            )
        else:
            self._common_packet_header = common_packet_header
        super().__init__(payloads=payloads, pathway=pathway,
                         source=source, raw=raw, endianness_code=endianness_code)

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE
               ) -> IrisCommonPacket:
        """Construct a Iris Packet Object from Bytes."""

        cph_data = data[:CPH_SIZE]
        CPH = IrisCommonPacket.CommonPacketHeader.decode(cph_data)
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
            #! TODO: handle uplink packets as well.
            #! TODO: Handle UplinkTimes/DownlinkTimes objects
            payloads = extract_downlinked_payloads(
                VLP=VLP,
                pathway=pathway,
                source=source
            )

        except Exception as e:
            trace = traceback.format_exc()
            logger.warning(
                f"Had to abort packet parsing due to the following exception: {trace}"
            )
            payloads = PayloadCollection.make_empty()

        return IrisCommonPacket(
            common_packet_header=CPH,
            payloads=payloads,
            pathway=pathway,
            source=source,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        # Lookup table that lists all payload types which should be encoded and
        # which Magics to use for them:
        encoded_payload_magics_lookup = {
            'CommandPayload': Magic.COMMAND.encode(),
            'TelemetryPayload': Magic.TELEMETRY.encode(),
            'EventPayload': Magic.EVENT.encode(),
            'FileBlockPayload': Magic.FILE.encode()
        }

        # Compile all payloads:
        VLP = b''
        for payload_type in encoded_payload_magics_lookup.keys():
            for cp in getattr(self.payloads, payload_type):
                VLP += cp.magic.encode() + cp.encode()

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
    @last-updated: 10/02/2020
    """

    LEGACY2020_CPH_SIZE: int = 5

    class Legacy2020CommonPacketHeaderInterface(ContainerCodec[CT]):
        pass

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
    def _count_vlp_len(payloads: PayloadCollection) -> int:
        """
        Calculates the VLP length necessary to send the given PayloadsCollection.
        Sum of all Payload sizes + MAGIC_SIZE * num_payloads
        """
        total_size = 0
        num_payloads = 0
        for c in payloads:
            c = cast(List, c)  # mypy doesn't get this by itself
            for p in c:
                total_size += len(p.encode())
                num_payloads += 1
        total_size += num_payloads * MAGIC_SIZE  # Each will require a magic
        return total_size

    def __str__(self) -> str:
        base = self.__repr__()

        # Grab the string of the lastest value for each unique telemetry channel:
        latest = {}
        for payload in self.payloads.TelemetryPayload:
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
            f"\t{len(self.payloads.TelemetryPayload)} T"  # Telemetry
            f"\t- {len(self.payloads.EventPayload)} E"
            f"\t- {len(self.payloads.FileBlockPayload)} B"  # File Blocks
            f"\t- {len(self.payloads.CommandPayload)} C"
        )

    def __init__(self,
                 payloads: PayloadCollection,
                 seq_num: int = 0,
                 common_packet_header: Optional[Legacy2020CommonPacketHeader] = None,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
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
        super().__init__(payloads=payloads, pathway=pathway,
                         source=source, raw=raw, endianness_code=endianness_code)

    @classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE
               ) -> Legacy2020IrisCommonPacket:
        """Construct a Iris Packet Object from Bytes."""

        cph_data = data[:Legacy2020IrisCommonPacket.LEGACY2020_CPH_SIZE]
        CPH = Legacy2020IrisCommonPacket.Legacy2020CommonPacketHeader.decode(
            cph_data)
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
            # Ignoring uplink and UplinkTimes/DownlinkTimes objects for
            # Legacy2020.
            payloads = extract_downlinked_payloads(
                VLP=VLP,
                pathway=pathway,
                source=source
            )

        except Exception as e:
            trace = traceback.format_exc()
            logger.warning(
                f"Had to abort packet parsing due to the following exception: {trace}"
            )
            payloads = PayloadCollection.make_empty()

        return Legacy2020IrisCommonPacket(
            common_packet_header=CPH,
            payloads=payloads,
            pathway=pathway,
            source=source,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        # Lookup table that lists all payload types which should be encoded and
        # which Magics to use for them:
        encoded_payload_magics_lookup = {
            'CommandPayload': Magic.COMMAND.encode(),
            'TelemetryPayload': Magic.TELEMETRY.encode(),
            'EventPayload': Magic.EVENT.encode(),
            'FileBlockPayload': Magic.FILE.encode()
        }

        # Compile all payloads:
        VLP = b''
        for payload_type in encoded_payload_magics_lookup.keys():
            for cp in getattr(self.payloads, payload_type):
                VLP += cp.magic.encode() + cp.encode()

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


class IrisDownlinkPacket(IrisCommonPacket):
    """
    Defines Common Data Required for All Downlinked Packets (Moon to Earth).

    All the same core data as any other IrisPacket but with special additional
    metadata coming from

    @author: Connor W. Colombo (CMU)
    @last-updated: 12/25/2020
    """

    __slots__: List[str] = [
        'time_received',
        'time_sent'
    ]

    def __init__(self) -> None:
        pass


class IrisUplinkPacket(Packet):
    """
    Defines Common Data Required for All Uplinked Packets (Earth to Moon).

    @author: Connor W. Colombo (CMU)
    @last-updated: 12/25/2020
    """

    __slots__: List[str] = [
        'time_issued',
        'time_logged',
        'time_processed',
        'time_sent',
        'time_confirmed',
        'time_received'
    ]

    def __init__(self) -> None:
        pass


class CustomPayloadPacket(Packet[CT]):
    """
    Superclass for a special **non**-Iris Common Packet packet
    (e.g. generated by the Watchdog) which contains items which would normally
    be in separate telemetry channels but instead are contained in one
    continuous `CustomPayload`. This class maps the extracts the values in the
    `CustomPayload` and stores them in traditional `TelemetryPayload`s linked to
    a custom `TelemetryChannel`.
    All of these telemetry channels must be found
    in a `DataStandards` module, typically a `prebuilt` one.

    **The subclass must specify a `PREBUILT_MODULE_NAME` which links the subclass
    to the `DataStandards` module it represents.**

    Members of the custom payload must have same names as the corresponding
    telemetry channels in the prebuilt `DataStandards` module.
    Note: an error will get thrown by `WatchdogCommandResponsePacket.__init__`
    when building `payloads` from the `custom_payload` if all the channels
    in the `WatchdogCommandResponse` prebuilt module don't have a corresponding
    attr in this `CustomPayload`.

    *NOTE:* For this to work effectively, all fields, including computed
    properties, in the custom payload must match the names from the prebuilt
    module AND the order of the args in `__init__` of the custom payload must
    match the order of the bytes in the packet.
    """
    PREBUILT_MODULE_NAME: Optional[str] = None

    __slots__: List[str] = [
        'custom_payload'
    ]
    custom_payload: object

    @classmethod
    def get_ds_module(cls) -> Module:
        """
        Returns the corresponding prebuilt `DataStandards` `Module` used for
        mapping this packet's data to telemetry streams.
        """
        if cls.PREBUILT_MODULE_NAME is None:
            raise NotImplementedError(
                f"{cls} is a subclass of `CustomPayloadPacket` "
                "which must provide a `PREBUILT_MODULE_NAME` class variable "
                "but doesn't."
            )

        try:
            return settings['STANDARDS'].modules[cls.PREBUILT_MODULE_NAME]
        except KeyError:
            raise TypeError(
                f"Attempted to parse a `{cls.__name__}` which "
                f"requires the `{cls.PREBUILT_MODULE_NAME}` special `prebuilt` "
                "module to be loaded into the standards but it can't be found."
            )

    def __init__(self,
                 custom_payload: object,
                 pathway: DataPathway = DataPathway.NONE,
                 source: DataSource = DataSource.NONE,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:

        self.custom_payload = custom_payload

        # Autopopulate payloads based on name:
        payloads: PayloadCollection = PayloadCollection(
            CommandPayload=[],
            TelemetryPayload=[],
            EventPayload=[],
            FileBlockPayload=[]
        )

        # Go through every telemetry channel in the linked `DataStandards`
        # `Module` and lookup the value in the Custom Payload.
        module = self.get_ds_module()
        for channel in module.telemetry.vals:
            try:
                payloads.TelemetryPayload.append(TelemetryPayload(
                    module_id=module.ID,
                    channel_id=channel.ID,
                    data=getattr(custom_payload, channel.name),
                    timestamp=int(time.time()),
                    magic=Magic.TELEMETRY,
                    pathway=pathway,
                    source=source,
                    endianness_code=endianness_code
                ))
            except AttributeError:
                raise NotImplementedError(
                    f"`CustomPayloadPacket` class {self.__class__} was given "
                    f"a `CustomPayload` of class {custom_payload.__class__} "
                    f"which should contain one attribute (as a `__dict__` "
                    "entry, `__slots__` entry, or computed `@property`) for "
                    "each `TelemetryChannel` in the given special `DataStandards` "
                    f"module: `{module}` *but* it is missing at least "
                    f"one: there is no attribute with the same name as the "
                    f"`TelemetryChannel`: {channel} in the `CustomPayload` "
                    f"class {custom_payload.__class__}."
                )

        super().__init__(payloads=payloads, pathway=pathway, source=source,
                         raw=raw, endianness_code=endianness_code)


class WatchdogTvacHeartbeatPacketInterface(CustomPayloadPacket[CT]):
    # Name of the corresponding prebuilt `Module` used for mapping this packet's
    # data to telemetry streams:
    PREBUILT_MODULE_NAME: str = 'WatchdogHeartbeatTvac'

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    class CustomPayload():
        """
        Core custom WatchdogTvacHeartbeat payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogHeartbeatTvac` prebuilt module.
        Note: an error will get thrown by `WatchdogTvacHeartbeatPacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogHeartbeatTvac` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.


        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet.
        """
        THERMISTOR_LOOKUP_TABLE = {  # for 5k thermistor: https://www.tdk-electronics.tdk.com/inf/50/db/ntc/NTC_Mini_sensors_S863.pdf
            'degC': np.asarray([-15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155]),
            'Vadc': np.asarray([4242, 3970, 3670, 3352, 3023, 2695, 2378, 2077, 1801, 1552, 1330, 1137, 969, 825, 702, 598, 510, 435, 372, 319, 274, 237, 204, 177, 154, 134, 117, 103, 90, 79, 70, 62, 55, 49, 44])
        }

        __slots__: List[str] = [
            '_AdcTempRaw',
            '_ChargeRaw',
            '_VoltageRaw',
            '_CurrentRaw',
            '_FuelTempRaw',
            '_KpHeater',
            '_HeaterSetpoint',
            '_HeaterWindow',
            '_HeaterPwmLimit',
            '_WatchdogMode',
            '_HeaterStatus',
            '_HeatingControlEnabled',
            '_HeaterPwmDutyCycle'
        ]

        _AdcTempRaw: int
        _ChargeRaw: int
        _VoltageRaw: int
        _CurrentRaw: int
        _FuelTempRaw: int
        _KpHeater: int
        _HeaterSetpoint: int
        _HeaterWindow: int
        _HeaterPwmLimit: int
        _WatchdogMode: int
        _HeaterStatus: int
        _HeatingControlEnabled: int
        _HeaterPwmDutyCycle: int

        @property
        def _module(self) -> Module:
            return WatchdogTvacHeartbeatPacketInterface.get_ds_module()

        @property
        def AdcTempKelvin(self) -> float:
            return float(np.interp(self._AdcTempRaw, self.THERMISTOR_LOOKUP_TABLE['Vadc'][::-1], self.THERMISTOR_LOOKUP_TABLE['degC'][::-1]) + 273.15)

        @property
        def ChargeMah(self) -> float:
            return 0  # Not implemented in TVAC Heartbeats (conversion unknown)

        @property
        def Voltage(self) -> float:
            return self._VoltageRaw * 0.00108033875

        @property
        def CurrentAmps(self) -> float:
            return 0.0000390636921 * (self._CurrentRaw - 32767)

        @property
        def FuelTempKelvin(self) -> float:
            return 0.00778210117 * self._FuelTempRaw

        @property
        def HeaterSetpointKelvin(self) -> float:
            return float(np.interp(self._HeaterSetpoint, self.THERMISTOR_LOOKUP_TABLE['Vadc'][::-1], self.THERMISTOR_LOOKUP_TABLE['degC'][::-1]) + 273.15)

        @property
        def HeaterWindowKelvin(self) -> float:
            upper = float(np.interp(self._HeaterSetpoint-self._HeaterWindow,
                          self.THERMISTOR_LOOKUP_TABLE['Vadc'][::-1], self.THERMISTOR_LOOKUP_TABLE['degC'][::-1]))
            lower = float(np.interp(self._HeaterSetpoint+self._HeaterWindow,
                          self.THERMISTOR_LOOKUP_TABLE['Vadc'][::-1], self.THERMISTOR_LOOKUP_TABLE['degC'][::-1]))
            return abs(upper-lower)/2

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)
        @property
        def AdcTempRaw(self) -> int: return self._AdcTempRaw
        @property
        def ChargeRaw(self) -> int: return self._ChargeRaw
        @property
        def VoltageRaw(self) -> int: return self._VoltageRaw
        @property
        def CurrentRaw(self) -> int: return self._CurrentRaw
        @property
        def FuelTempRaw(self) -> int: return self._FuelTempRaw
        @property
        def KpHeater(self) -> int: return self._KpHeater
        @property
        def HeaterSetpoint(self) -> int: return self._HeaterSetpoint
        @property
        def HeaterWindow(self) -> int: return self._HeaterWindow
        @property
        def HeaterPwmLimit(self) -> int: return self._HeaterPwmLimit
        @property
        def HeaterStatus(self) -> int: return self._HeaterStatus

        @property
        def WatchdogMode(self) -> int: return self._WatchdogMode

        @property
        def WatchdogModeName(self) -> str:
            return cast(str, self._module.telemetry['WatchdogMode'].get_enum_name(
                self.WatchdogMode
            ))

        @property
        def HeatingControlEnabled(self) -> int:
            return self._HeatingControlEnabled

        @property
        def HeaterPwmDutyCycle(self) -> int: return self._HeaterPwmDutyCycle

        def __init__(self,
                     AdcTempRaw: int,
                     ChargeRaw: int,
                     VoltageRaw: int,
                     CurrentRaw: int,
                     FuelTempRaw: int,
                     KpHeater: int,
                     HeaterSetpoint: int,
                     HeaterWindow: int,
                     HeaterPwmLimit: int,
                     WatchdogMode: Union[int, str],
                     HeaterStatus: int,
                     HeatingControlEnabled: int,
                     HeaterPwmDutyCycle: int
                     ) -> None:
            self._AdcTempRaw = AdcTempRaw
            self._ChargeRaw = struct.unpack(
                '>H', struct.pack('<H', ChargeRaw))[0]
            self._VoltageRaw = struct.unpack(
                '>H', struct.pack('<H', VoltageRaw))[0]
            self._CurrentRaw = struct.unpack(
                '>H', struct.pack('<H', CurrentRaw))[0]
            self._FuelTempRaw = struct.unpack(
                '>H', struct.pack('<H', FuelTempRaw))[0]
            self._KpHeater = KpHeater
            self._HeaterSetpoint = HeaterSetpoint
            self._HeaterWindow = HeaterWindow
            self._HeaterPwmLimit = HeaterPwmLimit
            self._HeaterStatus = HeaterStatus
            self._HeatingControlEnabled = HeatingControlEnabled
            self._HeaterPwmDutyCycle = HeaterPwmDutyCycle

            if isinstance(WatchdogMode, int):
                self._WatchdogMode = WatchdogMode
            else:
                self._WatchdogMode = cast(int, self._module.telemetry['WatchdogMode'].get_enum_value(
                    cast(str, WatchdogMode)
                ))

        def __repr__(self) -> str:
            return (
                f"{self.WatchdogModeName}:\t"
                f"[Heat: {'ON' if self.HeaterStatus else 'OFF'}, Ctrl: {'ON' if self.HeatingControlEnabled else 'OFF'}] \t"
                f"{self.AdcTempKelvin:.1f}°K -> "
                f"{self.HeaterSetpointKelvin:.1f}°K +- {self.HeaterWindowKelvin:.2f}K° \t"
                f"Kp = {self.KpHeater} @ Duty Cycle: {self.HeaterPwmDutyCycle}/{0xFFFF}"
            )


class WatchdogTvacHeartbeatPacket(WatchdogTvacHeartbeatPacketInterface[WatchdogTvacHeartbeatPacketInterface]):
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\xFF'  # Required start flag

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    def __repr__(self) -> str:
        return self.custom_payload.__repr__()

    @ classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE
               ) -> WatchdogTvacHeartbeatPacket:
        flag, core_data = data[:1], data[1:]
        if cls.START_FLAG != flag:
            raise PacketDecodingException(
                data,
                "Start flag for `WatchdogTvacHeartbeatPacket` was invalid. "  # type: ignore
                f"Expected {cls.START_FLAG}, Got: {flag} ."
            )
        custom_payload = WatchdogTvacHeartbeatPacket.CustomPayload(
            *struct.unpack(endianness_code + '9H 3B H', core_data)
        )
        return WatchdogTvacHeartbeatPacket(
            custom_payload=custom_payload,
            pathway=pathway,
            source=source,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        #! TODO (not really a typical use case so not super necessary besides for completeness)
        raise NotImplementedError()

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[:1] == cls.START_FLAG
        right_length = len(data) == 24  # Bytes

        return right_start and right_length


class WatchdogDetailedStatusPacketInterface(CustomPayloadPacket[CT]):
    # Name of the corresponding prebuilt `Module` used for mapping this packet's
    # data to telemetry streams:
    PREBUILT_MODULE_NAME: str = 'WatchdogDetailedStatus'

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    class CustomPayload():
        """
        Core custom WatchdogDetailedStatus payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogDetailedStatus` prebuilt module.
        Note: an error will get thrown by `WatchdogDetailedStatusPacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogDetailedStatus` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.


        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet.
        """
        #! TODO: Consider moving these (and even V divider values etc) into some common GSW "SYSTEM PROPERTIES" struct somewhere
        # TODO: 5k table taken from old Avionics conversion sheet. not yet checked/verified. (check it)
        BATT_5K_THERMISTOR_LOOKUP_TABLE = {  # for 5k thermistor: https://www.tdk-electronics.tdk.com/inf/50/db/ntc/NTC_Mini_sensors_S863.pdf
            'degC': np.asarray([-15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155]),
            'adc': np.asarray([4242, 3970, 3670, 3352, 3023, 2695, 2378, 2077, 1801, 1552, 1330, 1137, 969, 825, 702, 598, 510, 435, 372, 319, 274, 237, 204, 177, 154, 134, 117, 103, 90, 79, 70, 62, 55, 49, 44])
        }
        BATT_CHRG_10K_THERMISTOR_LOOKUP_TABLE = {  # for 10k thermistor (NTC10k_B57863S0103F040)
            'degC': np.asarray([-55, -50, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155]),
            'RTH_R25': np.asarray([96.3, 67.01, 47.17, 33.65, 24.26, 17.7, 13.04, 9.707, 7.293, 5.533, 4.232, 3.265, 2.539, 1.99, 1.571, 1.249, 1.0000, 0.8057, 0.6531, 0.5327, 0.4369, 0.3603, 0.2986, 0.2488, 0.2083, 0.1752, 0.1481, 0.1258, 0.1072, 0.09177, 0.07885, 0.068, 0.05886, 0.05112, 0.04454, 0.03893, 0.03417, 0.03009, 0.02654, 0.02348, 0.02083, 0.01853, 0.01653])
        }
        V_HEATER_NOM = 28.0  # NOMINAL Heater Voltage
        R_HEATER = 628.245  # Heater resistance (ohms)

        __slots__: List[str] = [
            '_Io_ChargingStatus1',
            '_Io_ChargingStatus2',
            '_Io_BatteryConnectionStatus',
            '_Io_BatteryLatchStatus',
            '_Io_1V2PowerGood',
            '_Io_1V8PowerGood',
            '_Io_3V3PowerGood',
            '_Io_5V0PowerGood',
            '_Watchdog_State',
            '_Watchdog_DeploymentStatus',
            '_Watchdog_Uart0State',
            '_Watchdog_Uart1State',
            '_Adc_BatteryTempRaw',
            '_Watchdog_DetailedHeartbeatSequenceNumber',
            '_Watchdog_DigitalOutputStates',
            '_Watchdog_ResetLogs',
            '_Adc_LanderVoltageRaw',
            '_Adc_BatteryChargingTempRaw',
            '_Adc_FullSystemVoltageRaw',
            '_Adc_FullSystemCurrentRaw',
            '_Adc_SwitchedBatteryVoltageRaw',
            '_Adc_Vcc24VoltageRaw',
            '_Heater_ControlEnabled',
            '_Heater_IsHeating',
            '_Adc_2V5VoltageRaw',
            '_Adc_2V8VoltageRaw',
            '_Adc_Vcc28VoltageRaw',
            '_Heater_Kp',
            '_Heater_PwmLimit_DutyCycleCounter',
            '_Heater_SetpointValue',
            '_Heater_OnValue',
            '_Heater_OffValue',
            '_Heater_DutyCyclePeriodCycles',
            '_Heater_DutyCycleCounter',
            '_I2C_BatteryChargeRaw',
            '_I2C_BatteryVoltageRaw',
            '_I2C_BatteryCurrentRaw',
            '_I2C_FuelGaugeTempRaw',
            '_I2C_BatteryChargeTelemRaw',
            '_I2C_BatteryCurrentTelemRaw'
        ]

        _Io_ChargingStatus1: int
        _Io_ChargingStatus2: int
        _Io_BatteryConnectionStatus: int
        _Io_BatteryLatchStatus: int
        _Io_1V2PowerGood: int
        _Io_1V8PowerGood: int
        _Io_3V3PowerGood: int
        _Io_5V0PowerGood: int
        _Watchdog_State: int
        _Watchdog_DeploymentStatus: int
        _Watchdog_Uart0State: int
        _Watchdog_Uart1State: int
        _Adc_BatteryTempRaw: int
        _Watchdog_DetailedHeartbeatSequenceNumber: int
        _Watchdog_DigitalOutputStates: int
        _Watchdog_ResetLogs: int
        _Adc_LanderVoltageRaw: int
        _Adc_BatteryChargingTempRaw: int
        _Adc_FullSystemVoltageRaw: int
        _Adc_FullSystemCurrentRaw: int
        _Adc_SwitchedBatteryVoltageRaw: int
        _Adc_Vcc24VoltageRaw: int
        _Heater_ControlEnabled: int
        _Heater_IsHeating: int
        _Adc_2V5VoltageRaw: int
        _Adc_2V8VoltageRaw: int
        _Adc_Vcc28VoltageRaw: int
        _Heater_Kp: int
        _Heater_PwmLimit_DutyCycleCounter: int
        _Heater_SetpointValue: int
        _Heater_OnValue: int
        _Heater_OffValue: int
        _Heater_DutyCyclePeriodCycles: int
        _Heater_DutyCycleCounter: int
        _I2C_BatteryChargeRaw: int
        _I2C_BatteryVoltageRaw: int
        _I2C_BatteryCurrentRaw: int
        _I2C_FuelGaugeTempRaw: int
        _I2C_BatteryChargeTelemRaw: int
        _I2C_BatteryCurrentTelemRaw: int

        @property
        def _module(self) -> Module:
            return WatchdogDetailedStatusPacketInterface.get_ds_module()

        # Helpful computed properties (which don't end up in Computed TelemetryChannels):
        @property
        def mean_lander_voltage(self) -> float:
            return self.Adc_LanderVoltage / 2.0 + self.Adc_Vcc28Voltage / 2.0

        @property
        def fused_est_lander_voltage(self) -> float:
            """
            Fuses two sensor readings weighted based on their relative 
            uncertainties to estimated the true lander voltage.
            """
            # TODO: Update these weights from empirical measurements of their accuracies and uncertainties
            dLander = 0.25  # [Volts] (uncertainty in LanderVoltage reading)
            dVcc28 = 0.5  # [Volts] (uncertainty in Vcc28Voltage reading)
            if (dLander+dVcc28) == 0:
                return 0
            else:
                return self.Adc_LanderVoltage * (1.0-dLander/(dLander+dVcc28)) + self.Adc_Vcc28Voltage * (1.0-dVcc28/(dLander+dVcc28))

        # Computed properties for Computed Telemetry Channels:
        @property
        def Io_ChargerState(self) -> int:
            return self.Io_ChargingStatus1 << 1 | self.Io_ChargingStatus2

        @property
        def Io_BatteryState(self) -> int:
            return self.Io_BatteryConnectionStatus << 1 | self.Io_BatteryLatchStatus

        @property
        def Watchdog_DigitalOutputStates_Dict(self) -> OrderedDict[str, int]:
            data = self.Watchdog_DigitalOutputStates.to_bytes(4, 'big')
            # unfortunately we can't just feed `to_bytes(4, 'little')` into
            # `bitstruct` because it expects the order of the bits in each byte
            # to be the left to right but MSP fills right to left. So, we have
            # to either convert to little then flip the order of the bits in
            # each byte or convert to big and flip the order of all the bits:
            data = flip_all_bits_in_bytes(data)

            bitfields = self._module.telemetry['Watchdog_DigitalOutputStates'].bitfields

            if bitfields is not None:
                return bitfields.unpack(data)
            else:
                return OrderedDict()

        @property
        def Watchdog_CombinedDigitalStates_Dict(self) -> OrderedDict[str, int]:
            """
            Creates a dict of states for each pin where:
                0 = output driven low
                1 = output driven high
                2 = input (Hi-Z)
            """
            # Make a new dict with all entries present in bitfield definition:
            bitfields = self._module.telemetry['Watchdog_CombinedDigitalStates'].bitfields
            if bitfields is not None:
                fields = bitfields.fields.copy()

            # Init all entries with a value indicating they haven't been processed yet:
            for k in fields.keys():
                fields[k] = -1

            # Grab states from Watchdog_DigitalOutputStates:
            out_states = self.Watchdog_DigitalOutputStates_Dict
            # Transfer over all states that exist in both:
            for k in fields.keys():
                fields[k] = out_states['OPSBI__'+k]

            # Build special (input capable) states:
            fields['CHRG_EN'] = 1 if out_states['OPSBI__CHRG_EN_FORCE_HIGH'] else out_states['OPSBI__CHRG_EN'] * 2
            if out_states['OPSBI__V_SYS_ALL_EN_FORCE_LOW']:
                fields['V_SYS_ALL_EN'] = 0
            else:
                fields['V_SYS_ALL_EN'] = 1 if out_states['OPSBI__V_SYS_ALL_EN'] else 2

            fields['RADIO_N_RST'] = 2 if out_states['OPSBI__RADIO_N_RESET_IS_INPUT'] else out_states['OPSBI__RADIO_N_RST']
            fields['HERCULES_N_RST'] = 2 if out_states['OPSBI__HERCULES_N_RST_IS_INPUT'] else out_states['OPSBI__HERCULES_N_RST']
            fields['HERCULES_N_PORRST'] = 2 if out_states['OPSBI__HERCULES_N_PORRST_IS_INPUT'] else out_states['OPSBI__HERCULES_N_PORRST']
            fields['FPGA_N_RST'] = 2 if out_states['OPSBI__FPGA_N_RST_IS_INPUT'] else out_states['OPSBI__FPGA_N_RST']
            fields['LATCH_SET'] = 2 if out_states['OPSBI__LATCH_SET_IS_INPUT'] else out_states['OPSBI__LATCH_SET']
            fields['LATCH_RESET'] = 2 if out_states['OPSBI__LATCH_RESET_IS_INPUT'] else out_states['OPSBI__LATCH_RESET']
            fields['BATT_STAT'] = 2 if out_states['OPSBI__BATT_STAT_IS_INPUT'] else out_states['OPSBI__BATT_STAT']

            # Check for any unprocessed fields:
            if any(v < 0 for v in fields.values()):
                raise ValueError(
                    "In `WatchdogDetailedStatus`, not all fields in "
                    "`Watchdog_CombinedDigitalStates` have a matching field in "
                    "`Watchdog_DigitalOutputStates`. "
                    "`-1` indicates unmatched in the following: "
                    f"Watchdog_CombinedDigitalStates: {fields}, "
                    f"Watchdog_DigitalOutputStates: {out_states} ."
                )

            return fields

        @property
        def Watchdog_CombinedDigitalStates(self) -> int:
            bitfields = self._module.telemetry['Watchdog_CombinedDigitalStates'].bitfields
            if bitfields is not None:
                raw_data = bitfields.pack(
                    self.Watchdog_CombinedDigitalStates_Dict)
                # flip order of all bits to be consistent with `Watchdog_DigitalOutputStates_Dict` decoding:
                raw_data = flip_all_bits_in_bytes(raw_data)
                return int.from_bytes(raw_data, 'big', signed=False)
            else:
                return 0

        @property
        def Watchdog_CombinedDigitalStates_Shorthand(self) -> OrderedDict[str, int]:
            """
            Returns a dict containing all fields from `Watchdog_CombinedDigitalStates_Dict`
            but using shorthand aliases for each pin (for easy printing):
            """
            long_form = self.Watchdog_CombinedDigitalStates_Dict
            return OrderedDict([
                ('HEAT', long_form['HEATER']),
                ('DEP', long_form['DEPLOYMENT']),

                ('BSTAT', long_form['BATT_STAT']),

                ('BE', long_form['BATTERY_EN']),
                ('LB', long_form['LATCH_BATT']),
                ('LS', long_form['LATCH_SET']),
                ('LR', long_form['LATCH_RESET']),

                ('CE', long_form['CHRG_EN']),
                ('REGE', long_form['V_LANDER_REG_EN']),

                ('VSAE', long_form['V_SYS_ALL_EN']),

                ('3V3_EN', long_form['3V3_EN']),

                ('H_ON', long_form['HERCULES_ON']),
                ('R_ON', long_form['RADIO_ON']),
                ('F_ON', long_form['FPGA_ON']),
                ('M_ON', long_form['MOTOR_ON']),

                ('H_RST', long_form['HERCULES_N_RST']),
                ('H_N_PRST', long_form['HERCULES_N_PORRST']),
                ('F_N_RST', long_form['FPGA_N_RST']),
                ('R_N_RST', long_form['RADIO_N_RST']),

                ('CAMSEL', long_form['FPGA_KICK_AKA_CAM_SELECT']),
                ('BMSB', long_form['BMS_BOOT'])
            ])

        @property
        def Watchdog_CombinedDigitalStates_DataFrame(self) -> DataFrame:
            """
            Returns a Pandas DataFrame containing all fields from 
            `Watchdog_CombinedDigitalStates_Dict` using shorthand aliases for 
            each pin from `Watchdog_CombinedDigitalStates_Shorthand` for the 
            column headers.
            """
            return DataFrame(self.Watchdog_CombinedDigitalStates_Shorthand, index=['GPIO (2=HiZ)'])

        @property
        def Watchdog_ResetLogs_Dict(self) -> OrderedDict[str, int]:
            data = self.Watchdog_ResetLogs.to_bytes(40//8, 'big')
            # unfortunately we can't just feed `to_bytes(5, 'little')` into
            # `bitstruct` because it expects the order of the bits in each byte
            # to be the left to right but MSP fills right to left. So, we have
            # to either convert to little then flip the order of the bits in
            # each byte or convert to big and flip the order of all the bits:
            data = flip_all_bits_in_bytes(data)

            bitfields = self._module.telemetry['Watchdog_ResetLogs'].bitfields
            if bitfields is not None:
                return bitfields.unpack(data)
            else:
                return OrderedDict()

        @property
        def Watchdog_ResetLogs_Dict_Shorthand(self) -> OrderedDict[str, int]:
            """
            Returns a dict containing all fields from `Watchdog_ResetLogs_Dict`
            but using shorthand aliases for each pin (for easy printing):
            """
            long_form = self.Watchdog_ResetLogs_Dict
            return OrderedDict([
                ('NONE', long_form['RABI__NO_RESET']),

                ('HDRM_SIG_ON', long_form['RABI__HDRM_DEPLOY_SIGNAL_POWER_ON']),  # noqa (don't wrap this line around)
                ('HDRM_SIG_OFF', long_form['RABI__HDRM_DEPLOY_SIGNAL_POWER_OFF']),  # noqa (don't wrap this line around)

                ('BATT_EN', long_form['RABI__BATTERIES_ENABLE']),
                ('BATT_DIS', long_form['RABI__BATTERIES_DISABLE']),
                ('CHARGE_START', long_form['RABI__BATTERY_CHARGE_START']),
                ('CHARGE_STOP', long_form['RABI__BATTERY_CHARGE_STOP']),

                ('HEAT_CTRL_EN', long_form['RABI__AUTO_HEATER_CONTROLLER_ENABLE']),  # noqa (don't wrap this line around)
                ('HEAT_CTRL_DIS', long_form['RABI__AUTO_HEATER_CONTROLLER_DISABLE']),  # noqa (don't wrap this line around)

                ('RS422_EN', long_form['RABI__RS422_UART_ENABLE']),
                ('RS422_DIS', long_form['RABI__RS422_UART_DISABLE']),

                ('3V3_EN_RST', long_form['RABI__3V3_EN_RESET']),
                ('3V3_EN_UNRST', long_form['RABI__3V3_EN_UNRESET']),
                ('3V3_EN_ON', long_form['RABI__3V3_EN_POWER_ON']),
                ('3V3_EN_OFF', long_form['RABI__3V3_EN_POWER_OFF']),

                ('H_RST', long_form['RABI__HERCULES_RESET']),
                ('H_UNRST', long_form['RABI__HERCULES_UNRESET']),
                ('H_ON', long_form['RABI__HERCULES_POWER_ON']),
                ('H_OFF', long_form['RABI__HERCULES_POWER_OFF']),
                ('H_WD_EN', long_form['RABI__HERCULES_WATCHDOG_ENABLE']),
                ('H_WD_DIS', long_form['RABI__HERCULES_WATCHDOG_DISABLE']),
                ('H_WD_RST', long_form['RABI__HERCULES_WATCHDOG_RESET']),

                ('R_RST', long_form['RABI__RADIO_RESET']),
                ('R_UNRST', long_form['RABI__RADIO_UNRESET']),
                ('R_ON', long_form['RABI__RADIO_POWER_ON']),
                ('R_OFF', long_form['RABI__RADIO_POWER_OFF']),

                ('F_RST', long_form['RABI__CAM_FPGA_RESET']),
                ('F_UNRST', long_form['RABI__CAM_FPGA_UNRESET']),
                ('F_ON', long_form['RABI__CAM_FPGA_POWER_ON']),
                ('F_OFF', long_form['RABI__CAM_FPGA_POWER_OFF']),

                ('CAM0_SEL', long_form['RABI__FPGA_CAM_0_SELECT']),
                ('CAM1_SEL', long_form['RABI__FPGA_CAM_1_SELECT']),

                ('ALL_MOT_ON', long_form['RABI__ALL_MOTORS_POWER_ON']),
                ('ALL_MOT_OFF', long_form['RABI__ALL_MOTORS_POWER_OFF']),
                ('24V_EN_RST', long_form['RABI__24V_EN_RESET']),
                ('24V_EN_UNRST', long_form['RABI__24V_EN_UNRESET']),
                ('24V_EN_ON', long_form['RABI__24V_EN_POWER_ON']),
                ('24V_EN_OFF', long_form['RABI__24V_EN_POWER_OFF'])
            ])

        @property
        def Watchdog_ResetLogs_Dict_DataFrame(self) -> DataFrame:
            """
            Returns a Pandas DataFrame containing all fields from 
            `Watchdog_ResetLogs_Dict_Dict` using shorthand aliases for 
            each pin from `Watchdog_ResetLogs_Dict_Shorthand` for the 
            column headers.
            """
            return DataFrame(self.Watchdog_ResetLogs_Dict_Shorthand, index=['Flag'])

        @property
        def Watchdog_ResetEventsList(self) -> List[str]:
            """
            Returns a list of the names of all ResetLogs that have a non-zero 
            value (i.e. the event happened).
            """
            return [name.replace('RABI__', '') for name, val in self.Watchdog_ResetLogs_Dict.items() if val != 0]  # noqa (don't wrap this line around)

        @property
        def Adc_LanderVoltage(self) -> float:
            return float(self.Adc_LanderVoltageRaw) / 4095.0 * 3.3 * (232.0+2000.0)/232.0

        @property
        def Adc_BatteryChargingTempKelvin(self) -> float:
            R25 = 10e3
            RT1 = 4320
            RT2 = 19100
            if self.Adc_BatteryChargingTempRaw > 0:
                V_ADC = float(self.Adc_BatteryChargingTempRaw) / 4095.0 * 3.3
                R_TH = 1.0 / ((3.3 - V_ADC) / (V_ADC * RT1) - 1.0 / RT2)
                return float(np.interp(
                    R_TH/R25,
                    self.BATT_CHRG_10K_THERMISTOR_LOOKUP_TABLE['RTH_R25'][::-1],
                    self.BATT_CHRG_10K_THERMISTOR_LOOKUP_TABLE['degC'][::-1]
                )) + 273.15
            else:
                return 0

        @property
        def Adc_BatteryChargingTempUncertaintyKelvin(self) -> float:
            # ! TODO: (pull from BACK HAL code + supporting samsung notes)
            return 0.0

        def battery_adc_reading_to_kelvin(self, adc_reading) -> float:
            return float(np.interp(
                float(adc_reading),
                self.BATT_5K_THERMISTOR_LOOKUP_TABLE['adc'][::-1],
                self.BATT_5K_THERMISTOR_LOOKUP_TABLE['degC'][::-1]
            )) + 273.15

        @property
        def Adc_BatteryTempKelvin(self) -> float:
            full_adc_reading = self.Adc_BatteryTempRaw  # already 12b
            return self.battery_adc_reading_to_kelvin(full_adc_reading)

        @property
        def Adc_BatteryTempUncertaintyKelvin(self) -> float:
            # ! TODO: calculate. Use `Adc_BatteryChargingTempUncertaintyKelvin` for reference
            return 0.0

        @property
        def Adc_FullSystemVoltage(self) -> float:
            return float(self.Adc_FullSystemVoltageRaw) / 4095.0 * 3.3 * (232.0+2000.0)/232.0

        @property
        def Adc_FullSystemCurrent(self) -> float:
            # uses *bottom* 9b, so we don't have to shift it, it just saturates earlier:
            full_adc_reading = self.Adc_FullSystemCurrentRaw
            return float(full_adc_reading) / 4095 * 3.3 * 4600/1000

        @property
        def Adc_SwitchedBatteryVoltage(self) -> float:
            return float(self.Adc_SwitchedBatteryVoltageRaw) / 4095.0 * 3.3 * (274.0+2000.0)/274.0

        @property
        def Adc_2V5Voltage(self) -> float:
            # no divider (or protection resistor) used, we read it directly:
            return float(self.Adc_2V5VoltageRaw) / 4095.0 * 3.3

        @property
        def Adc_2V8Voltage(self) -> float:
            # no divider (or protection resistor) used, we read it directly:
            return float(self.Adc_2V8VoltageRaw) / 4095.0 * 3.3

        @property
        def Adc_Vcc28Voltage(self) -> float:
            return float(self.Adc_Vcc28VoltageRaw) / 4095.0 * 3.3 * (47.0+470.0)/47.0

        @property
        def Adc_Vcc24Voltage(self) -> float:
            return float(self.Adc_Vcc24VoltageRaw) / 4095.0 * 3.3 * (47.0+330.0)/47.0

        @property
        def Adc_DataFrame(self) -> DataFrame:
            """
            Creates a Pandas DataFrame that summarizes all ADC readings incl. 
            both their raw ADC readings and their conversions to human-readable 
            units.
            """
            return DataFrame({
                '2V5': [self.Adc_2V5VoltageRaw, f"{self.Adc_2V5Voltage:.2f}V"],
                '2V8': [self.Adc_2V8VoltageRaw, f"{self.Adc_2V8Voltage:.2f}V"],
                '24V': [self.Adc_Vcc24VoltageRaw, f"{self.Adc_Vcc24Voltage:.2f}V"],
                'VBm': [self.Adc_SwitchedBatteryVoltageRaw, f"{self.Adc_SwitchedBatteryVoltage:.2f}V"],
                'VL': [self.Adc_LanderVoltageRaw, f"{self.Adc_LanderVoltage:.2f}V"],
                'Vcc28': [self.Adc_Vcc28VoltageRaw, f"{self.Adc_Vcc28Voltage:.2f}V"],
                'VSA': [self.Adc_FullSystemVoltageRaw, f"{self.Adc_FullSystemVoltage:.2f}V"],
                'ISA': [self.Adc_FullSystemCurrentRaw, f"{self.Adc_FullSystemCurrent*1000:.1f}mA"],
                'Tbatt': [self.Adc_BatteryTempRaw, f"{self.Adc_BatteryTempKelvin:.1f}±{self.Adc_BatteryTempUncertaintyKelvin:.1f}K"],
                'Tchrg': [self.Adc_BatteryChargingTempRaw, f"{self.Adc_BatteryChargingTempKelvin:.1f}±{self.Adc_BatteryChargingTempUncertaintyKelvin:.1f}K"]
            }, index=['ADC', 'val'])

        @property
        def Heater_PwmLimit_DutyCyclePercent(self) -> float:
            if self.Heater_DutyCyclePeriodCycles == 0:
                return float('Inf')
            else:
                return 100.0 * float(self.Heater_PwmLimit_DutyCycleCounter) / float(self.Heater_DutyCyclePeriodCycles)

        @property
        def Heater_EffectivePowerLimit(self) -> float:
            # TODO: consider using V_HEATER_NOM instead of `fused_est_lander_voltage` in case `fused_est_lander_voltage` isn't accurate
            max_avail_voltage = self.fused_est_lander_voltage * \
                self.Heater_PwmLimit_DutyCyclePercent / 100.0
            if self.R_HEATER == 0:
                return float('Inf')
            else:
                return max_avail_voltage**2 / self.R_HEATER

        @property
        def Heater_SetpointKelvin(self) -> float:
            # ! TODO: Why are we sending 16b if Adc maxes out at 12b... Strongly consider revising.
            # already 12b (16b sent)
            full_adc_reading = self.Heater_SetpointValue
            return self.battery_adc_reading_to_kelvin(full_adc_reading)

        @property
        def Heater_OnTempKelvin(self) -> float:
            # ! TODO: Why are we sending 16b if Adc maxes out at 12b... Strongly consider revising.
            full_adc_reading = self.Heater_OnValue  # already 12b (16b sent)
            return self.battery_adc_reading_to_kelvin(full_adc_reading)

        @property
        def Heater_OffTempKelvin(self) -> float:
            # ! TODO: Why are we sending 16b if Adc maxes out at 12b... Strongly consider revising.
            full_adc_reading = self.Heater_OffValue  # already 12b (16b sent)
            return self.battery_adc_reading_to_kelvin(full_adc_reading)

        @property
        def Heater_DutyCyclePeriodMs(self) -> float:
            wd_pwm_clock_freq: float = 8e6
            return 1000.0 * float(self.Heater_DutyCyclePeriodCycles) / wd_pwm_clock_freq

        @property
        def Heater_DutyCyclePercent(self) -> float:
            if self.Heater_DutyCyclePeriodCycles == 0:
                return float('Inf')
            else:
                return 100.0 * float(self.Heater_DutyCycleCounter) / float(self.Heater_DutyCyclePeriodCycles)

        @property
        def Heater_EffectiveVoltage(self) -> float:
            # TODO: consider using V_HEATER_NOM instead of `fused_est_lander_voltage` in case `fused_est_lander_voltage` isn't accurate
            return self.fused_est_lander_voltage * self.Heater_DutyCyclePercent / 100.0

        @property
        def Heater_EffectivePower(self) -> float:
            if self.R_HEATER == 0:
                return float('Inf')
            else:
                return self.Heater_EffectiveVoltage**2 / self.R_HEATER

        @property
        def I2C_BatteryChargeMah(self) -> float:
            # ! TODO: Impl. me! Conversion currently unknown
            return 0.0

        @property
        def I2C_BatteryVoltage(self) -> float:
            # ! TODO: Verify this conversion. Was just pulled from old WD code.
            return self.I2C_BatteryVoltageRaw * 0.00108033875

        @property
        def I2C_BatteryCurrent(self) -> float:
            # ! TODO: Verify this conversion. Was just pulled from old WD code.
            return 0.0000390636921 * self.I2C_BatteryCurrentRaw

        @property
        def I2C_FuelGaugeTempKelvin(self) -> float:
            # ! TODO: Verify this conversion. Was just pulled from old WD code.
            return 0.00778210117 * self.I2C_FuelGaugeTempRaw

        @property
        def I2C_BatteryChargeTelemMah(self) -> float:
            # ! TODO: Impl. me! Conversion currently unknown
            # TODO: Do we even need to send this? (isn't it covered by the above I2C readings?)
            return 0.0

        @property
        def I2C_BatteryCurrentTelemAmps(self) -> float:
            # ! TODO: Impl. me! Conversion currently unknown
            # TODO: Do we even need to send this? (isn't it covered by the above I2C readings?)
            return 0.0

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)

        @property
        def Io_ChargingStatus1(self) -> int: return self._Io_ChargingStatus1
        @property
        def Io_ChargingStatus2(self) -> int: return self._Io_ChargingStatus2

        @property
        def Io_BatteryConnectionStatus(self) -> int:
            return self._Io_BatteryConnectionStatus

        @property
        def Io_BatteryLatchStatus(self) -> int:
            return self._Io_BatteryLatchStatus

        @property
        def Io_1V2PowerGood(self) -> int: return self._Io_1V2PowerGood
        @property
        def Io_1V8PowerGood(self) -> int: return self._Io_1V8PowerGood
        @property
        def Io_3V3PowerGood(self) -> int: return self._Io_3V3PowerGood
        @property
        def Io_5V0PowerGood(self) -> int: return self._Io_5V0PowerGood
        @property
        def Watchdog_State(self) -> int: return self._Watchdog_State

        @property
        def Watchdog_DeploymentStatus(self) -> int:
            return self._Watchdog_DeploymentStatus

        @property
        def Watchdog_Uart0State(self) -> int: return self._Watchdog_Uart0State
        @property
        def Watchdog_Uart1State(self) -> int: return self._Watchdog_Uart1State
        @property
        def Adc_BatteryTempRaw(self) -> int: return self._Adc_BatteryTempRaw

        @property
        def Watchdog_DetailedHeartbeatSequenceNumber(self) -> int:
            return self._Watchdog_DetailedHeartbeatSequenceNumber

        @property
        def Watchdog_DigitalOutputStates(self) -> int:
            return self._Watchdog_DigitalOutputStates

        @property
        def Watchdog_ResetLogs(self) -> int: return self._Watchdog_ResetLogs

        @property
        def Adc_LanderVoltageRaw(self) -> int:
            return self._Adc_LanderVoltageRaw

        @property
        def Adc_BatteryChargingTempRaw(self) -> int:
            return self._Adc_BatteryChargingTempRaw

        @property
        def Adc_FullSystemVoltageRaw(self) -> int:
            return self._Adc_FullSystemVoltageRaw

        @property
        def Adc_FullSystemCurrentRaw(self) -> int:
            return self._Adc_FullSystemCurrentRaw

        @property
        def Adc_SwitchedBatteryVoltageRaw(self) -> int:
            return self._Adc_SwitchedBatteryVoltageRaw

        @property
        def Adc_Vcc24VoltageRaw(self) -> int: return self._Adc_Vcc24VoltageRaw

        @property
        def Heater_ControlEnabled(self) -> int:
            return self._Heater_ControlEnabled

        @property
        def Heater_IsHeating(self) -> int: return self._Heater_IsHeating
        @property
        def Adc_2V5VoltageRaw(self) -> int: return self._Adc_2V5VoltageRaw
        @property
        def Adc_2V8VoltageRaw(self) -> int: return self._Adc_2V8VoltageRaw
        @property
        def Adc_Vcc28VoltageRaw(self) -> int: return self._Adc_Vcc28VoltageRaw
        @property
        def Heater_Kp(self) -> int: return self._Heater_Kp

        @property
        def Heater_PwmLimit_DutyCycleCounter(self) -> int:
            return self._Heater_PwmLimit_DutyCycleCounter

        @property
        def Heater_SetpointValue(self) -> int:
            return self._Heater_SetpointValue

        @property
        def Heater_OnValue(self) -> int: return self._Heater_OnValue
        @property
        def Heater_OffValue(self) -> int: return self._Heater_OffValue

        @property
        def Heater_DutyCyclePeriodCycles(self) -> int:
            return self._Heater_DutyCyclePeriodCycles

        @property
        def Heater_DutyCycleCounter(self) -> int:
            return self._Heater_DutyCycleCounter

        @property
        def I2C_BatteryChargeRaw(self) -> int:
            return self._I2C_BatteryChargeRaw

        @property
        def I2C_BatteryVoltageRaw(self) -> int:
            return self._I2C_BatteryVoltageRaw

        @property
        def I2C_BatteryCurrentRaw(self) -> int:
            return self._I2C_BatteryCurrentRaw

        @property
        def I2C_FuelGaugeTempRaw(self) -> int:
            return self._I2C_FuelGaugeTempRaw

        @property
        def I2C_BatteryChargeTelemRaw(
            self) -> int: return self._I2C_BatteryChargeTelemRaw

        @property
        def I2C_BatteryCurrentTelemRaw(self) -> int:
            return self._I2C_BatteryCurrentTelemRaw

        # Name accessors for all enum-related params:
        def getEnumName(self, field_name) -> str:
            """
            Gets the name associated with the int value of the enum with the
            given `field_name`.
            Example: `getEnumName(Watchdog_State)` loads the int value of
            `Watchdog_State` and converts it a string as given by the enum for
            the `Watchdog_State` `TelemetryChannel` in the `Module` associated
            with this packet. Works for computed enum channels too (e.g.
            `Io_ChargerState`).
            """
            return cast(str, self._module.telemetry[field_name].get_enum_name(
                getattr(self, field_name)
            ))

        def __init__(self,
                     Io_ChargingStatus1: int,
                     Io_ChargingStatus2: int,
                     Io_BatteryConnectionStatus: Union[int, str],
                     Io_BatteryLatchStatus: Union[int, str],
                     Io_1V2PowerGood: Union[int, str],
                     Io_1V8PowerGood: Union[int, str],
                     Io_3V3PowerGood: Union[int, str],
                     Io_5V0PowerGood: Union[int, str],
                     Watchdog_State: Union[int, str],
                     Watchdog_DeploymentStatus: Union[int, str],
                     Watchdog_Uart0State: Union[int, str],
                     Watchdog_Uart1State: Union[int, str],
                     Adc_BatteryTempRaw: int,
                     Watchdog_DetailedHeartbeatSequenceNumber: int,
                     Watchdog_DigitalOutputStates: int,
                     Watchdog_ResetLogs: int,
                     Adc_LanderVoltageRaw: int,
                     Adc_BatteryChargingTempRaw: int,
                     Adc_FullSystemVoltageRaw: int,
                     Adc_FullSystemCurrentRaw: int,
                     Adc_SwitchedBatteryVoltageRaw: int,
                     Adc_Vcc24VoltageRaw: int,
                     Heater_ControlEnabled: Union[int, str],
                     Heater_IsHeating: Union[int, str],
                     Adc_2V5VoltageRaw: int,
                     Adc_2V8VoltageRaw: int,
                     Adc_Vcc28VoltageRaw: int,
                     Heater_Kp: int,
                     Heater_PwmLimit_DutyCycleCounter: int,
                     Heater_SetpointValue: int,
                     Heater_OnValue: int,
                     Heater_OffValue: int,
                     Heater_DutyCyclePeriodCycles: int,
                     Heater_DutyCycleCounter: int,
                     I2C_BatteryChargeRaw: int,
                     I2C_BatteryVoltageRaw: int,
                     I2C_BatteryCurrentRaw: int,
                     I2C_FuelGaugeTempRaw: int,
                     I2C_BatteryChargeTelemRaw: int,
                     I2C_BatteryCurrentTelemRaw: int
                     ) -> None:
            # TODO: Put all endianness flips and bitshifts here:

            self._Io_ChargingStatus1 = Io_ChargingStatus1
            self._Io_ChargingStatus2 = Io_ChargingStatus2
            self._Adc_BatteryTempRaw = Adc_BatteryTempRaw
            self._Watchdog_DetailedHeartbeatSequenceNumber = Watchdog_DetailedHeartbeatSequenceNumber
            self._Watchdog_DigitalOutputStates = Watchdog_DigitalOutputStates
            self._Watchdog_ResetLogs = Watchdog_ResetLogs
            self._Adc_LanderVoltageRaw = Adc_LanderVoltageRaw
            self._Adc_BatteryChargingTempRaw = Adc_BatteryChargingTempRaw
            self._Adc_FullSystemVoltageRaw = Adc_FullSystemVoltageRaw
            self._Adc_FullSystemCurrentRaw = Adc_FullSystemCurrentRaw
            self._Adc_SwitchedBatteryVoltageRaw = Adc_SwitchedBatteryVoltageRaw
            self._Adc_Vcc24VoltageRaw = Adc_Vcc24VoltageRaw
            self._Adc_2V5VoltageRaw = Adc_2V5VoltageRaw
            self._Adc_2V8VoltageRaw = Adc_2V8VoltageRaw
            self._Adc_Vcc28VoltageRaw = Adc_Vcc28VoltageRaw
            self._Heater_Kp = Heater_Kp
            self._Heater_PwmLimit_DutyCycleCounter = Heater_PwmLimit_DutyCycleCounter
            self._Heater_SetpointValue = Heater_SetpointValue
            self._Heater_OnValue = Heater_OnValue
            self._Heater_OffValue = Heater_OffValue
            self._Heater_DutyCyclePeriodCycles = Heater_DutyCyclePeriodCycles
            self._Heater_DutyCycleCounter = Heater_DutyCycleCounter
            self._I2C_BatteryChargeRaw = I2C_BatteryChargeRaw
            self._I2C_BatteryVoltageRaw = I2C_BatteryVoltageRaw
            self._I2C_BatteryCurrentRaw = I2C_BatteryCurrentRaw
            self._I2C_FuelGaugeTempRaw = I2C_FuelGaugeTempRaw
            self._I2C_BatteryChargeTelemRaw = I2C_BatteryChargeTelemRaw
            self._I2C_BatteryCurrentTelemRaw = I2C_BatteryCurrentTelemRaw

            def load_enum(field_name: str, val: Union[int, str]) -> None:
                """
                Allows an Enum field value to be given as an `int` of an 
                EnumItem value or `str` of an EnumItem name and ensures that the 
                `int` value is what's stored.
                """
                if isinstance(val, int):
                    setattr(self, '_'+field_name, val)
                else:
                    intval = cast(int, self._module.telemetry[field_name].get_enum_value(
                        cast(str, val)
                    ))
                    setattr(self, '_'+field_name, intval)

            load_enum('Io_BatteryConnectionStatus', Io_BatteryConnectionStatus)
            load_enum('Io_BatteryLatchStatus', Io_BatteryLatchStatus)
            load_enum('Io_1V2PowerGood', Io_1V2PowerGood)
            load_enum('Io_1V8PowerGood', Io_1V8PowerGood)
            load_enum('Io_3V3PowerGood', Io_3V3PowerGood)
            load_enum('Io_5V0PowerGood', Io_5V0PowerGood)
            load_enum('Watchdog_State', Watchdog_State)
            load_enum('Watchdog_DeploymentStatus', Watchdog_DeploymentStatus)
            load_enum('Watchdog_Uart0State', Watchdog_Uart0State)
            load_enum('Watchdog_Uart1State', Watchdog_Uart1State)
            load_enum('Heater_ControlEnabled', Heater_ControlEnabled)
            load_enum('Heater_IsHeating', Heater_IsHeating)

        def __repr__(self) -> str:
            return (
                f"#{self.Watchdog_DetailedHeartbeatSequenceNumber} "
                f"[{self.getEnumName('Watchdog_State')} + {self.getEnumName('Watchdog_DeploymentStatus')}] \t"
                f"CHARGER: {self.getEnumName('Io_ChargerState')} \t"
                f"BATTERIES: {self.getEnumName('Io_BatteryState')} \t"
                f"UART0: {self.getEnumName('Watchdog_Uart0State')} \t"
                f"UART1: {self.getEnumName('Watchdog_Uart1State')}"
                "\n"
                f"1V2: {self.getEnumName('Io_1V2PowerGood')} \t"
                f"1V8: {self.getEnumName('Io_1V8PowerGood')} \t"
                f"2V5: {self.Adc_2V5Voltage:.1f} \t"
                f"2V8: {self.Adc_2V8Voltage:.1f} \t"
                f"3V3: {self.getEnumName('Io_3V3PowerGood')} \t"
                f"5V0: {self.getEnumName('Io_5V0PowerGood')} \t"
                f"24V: {self.Adc_Vcc24Voltage:.2f}"
                "\n"
                f"VBm: {self.Adc_SwitchedBatteryVoltage:.2f}V \t"
                f"VL: {self.Adc_LanderVoltage:.2f}V \t"
                f"Vcc28: {self.Adc_Vcc28Voltage:.2f}V \t"
                f"VSA: {self.Adc_FullSystemVoltage:.2f}V \t"
                f"ISA: {self.Adc_FullSystemCurrent*1000:.1f}mA"
                "\n"
                "HEATER "
                f"[{self.Heater_OnTempKelvin:.0f}K |"
                f"{self.Heater_SetpointKelvin:.0f}K |"
                f"{self.Heater_OffTempKelvin:.0f}K] \t"
                f" is {self.getEnumName('Heater_IsHeating')}, "
                f"Control: {self.getEnumName('Heater_ControlEnabled')} \t"
                f"@ {self.Heater_EffectivePower:.3f}W / {self.Heater_EffectivePowerLimit:.3f}W \t"
                f"Period: {self.Heater_DutyCyclePeriodMs:.1f}ms"
                "\n"
                f"Tbatt: {self.Adc_BatteryTempKelvin:.1f}K ± {self.Adc_BatteryTempUncertaintyKelvin}K \t"
                f"Tchrg: {self.Adc_BatteryChargingTempKelvin:.1f}K ± {self.Adc_BatteryChargingTempUncertaintyKelvin}K \t"
                f"Tblimp: {self.I2C_FuelGaugeTempKelvin:.1f}K"
                "\n"
                "BATTERY MONITOR: "
                f"{self.I2C_BatteryVoltage:.2f}V \t"
                f"{self.I2C_BatteryCurrent*1000:.1f}mA \t"
                f"{self.I2C_BatteryChargeMah:.0f}mAh \t"
                f"\n{self.Adc_DataFrame.to_string(float_format=lambda x: '{:.2f}'.format(x))}"
                f"\n{self.Watchdog_CombinedDigitalStates_DataFrame.to_string()}"
                # f"GPIO (2=HiZ): {dict(**self.Watchdog_CombinedDigitalStates_Dict)} \n"
                f"\nReset/Flag Events: [{', '.join(self.Watchdog_ResetEventsList)}]"
                # f"\n{dict(**self.Watchdog_ResetLogs_Dict_Shorthand)}"
                # "\n"
                "\n"
            )


class WatchdogDetailedStatusPacket(WatchdogDetailedStatusPacketInterface[WatchdogDetailedStatusPacketInterface]):
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\xD5'  # Required start flag
    WD_ADC_BITS: int = 12

    # Bitfield Struct Allocations (represents the order and number of bits
    # assigned to all data in the message's bitfield struct):
    # Each field has the following entries: (n_bits, is_adc_val, n_upper_adc_bits, n_lower_adc_bits)
    BITFIELD_ALLOCATIONS: OrderedDict = OrderedDict([
        ('Io_ChargingStatus1', (1, False, 0, 0)),
        ('Io_ChargingStatus2', (1, False, 0, 0)),
        ('Io_BatteryConnectionStatus', (1, False, 0, 0)),
        ('Io_BatteryLatchStatus', (1, False, 0, 0)),
        ('Io_1V2PowerGood', (1, False, 0, 0)),
        ('Io_1V8PowerGood', (1, False, 0, 0)),
        ('Io_3V3PowerGood', (1, False, 0, 0)),
        ('Io_5V0PowerGood', (1, False, 0, 0)),
        ('Watchdog_State', (8, False, 0, 0)),
        ('Watchdog_DeploymentStatus', (2, False, 0, 0)),
        ('Watchdog_Uart0State', (1, False, 0, 0)),
        ('Watchdog_Uart1State', (1, False, 0, 0)),
        ('Adc_BatteryTempRaw', (12, True, WD_ADC_BITS, WD_ADC_BITS)),
        ('Watchdog_DetailedHeartbeatSequenceNumber', (8, False, 0, 0)),
        ('Watchdog_DigitalOutputStates', (32, False, 0, 0)),
        ('Watchdog_ResetLogs', (40, False, 0, 0)),
        ('Adc_LanderVoltageRaw', (7, True, 7, WD_ADC_BITS)),
        ('Adc_BatteryChargingTempRaw', (9, True, 9, WD_ADC_BITS)),
        ('Adc_FullSystemVoltageRaw', (5, True, 5, WD_ADC_BITS)),
        ('Adc_FullSystemCurrentRaw', (9, True, WD_ADC_BITS, 9)),
        ('Adc_SwitchedBatteryVoltageRaw', (9, True, 9, WD_ADC_BITS)),
        ('Adc_Vcc24VoltageRaw', (7, True, 7, WD_ADC_BITS)),
        ('Heater_ControlEnabled', (1, False, 0, 0)),
        ('Heater_IsHeating', (1, False, 0, 0)),
        ('Adc_2V5VoltageRaw', (5, True, 5, WD_ADC_BITS)),
        ('Adc_2V8VoltageRaw', (5, True, 5, WD_ADC_BITS)),
        ('Adc_Vcc28VoltageRaw', (6, True, 6, WD_ADC_BITS)),
        ('Heater_Kp', (16, False, 0, 0)),
        ('Heater_PwmLimit_DutyCycleCounter', (16, False, 0, 0)),
        ('Heater_SetpointValue', (16, False, 0, 0)),
        ('Heater_OnValue', (16, False, 0, 0)),
        ('Heater_OffValue', (16, False, 0, 0)),
        ('Heater_DutyCyclePeriodCycles', (16, False, 0, 0)),
        ('Heater_DutyCycleCounter', (16, False, 0, 0)),
        ('I2C_BatteryChargeRaw', (16, False, 0, 0)),
        ('I2C_BatteryVoltageRaw', (16, False, 0, 0)),
        ('I2C_BatteryCurrentRaw', (16, False, 0, 0)),
        ('I2C_FuelGaugeTempRaw', (16, False, 0, 0)),
        ('I2C_BatteryChargeTelemRaw', (8, False, 0, 0)),
        ('I2C_BatteryCurrentTelemRaw', (8, False, 0, 0))
    ])

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    def __repr__(self) -> str:
        return self.custom_payload.__repr__() + f' {hexstr(self.raw)}'

    @ classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE
               ) -> WatchdogDetailedStatusPacket:
        flag, core_data = data[:1], data[1:]
        if cls.START_FLAG != flag:
            raise PacketDecodingException(
                data,
                "Start flag for `WatchdogDetailedStatusPacket` was invalid. "  # type: ignore
                f"Expected {cls.START_FLAG}, Got: {flag} ."
            )

        # Create empty dict to contain extracted values for later use in destructuring in constructor call:
        named_bitfield_entries = dict([
            (k, 0) for k in cls.BITFIELD_ALLOCATIONS.keys()
        ])

        # Encode message as int and handle endianness:
        data_int: int = int.from_bytes(core_data, 'little', signed=False)

        # Extract all fields from `data_int` and shift adc fields as required:
        idx_bit_head: int = 0
        for field_name, (n_bits, is_adc_val, n_upper_adc_bits, _) in cls.BITFIELD_ALLOCATIONS.items():
            # TODO: consider storing the mask and idx_bit_head for each value after first run for faster processing (build once and store)
            field_mask = ((1 << n_bits) - 1) << idx_bit_head
            field_val = (data_int & field_mask) >> idx_bit_head

            if is_adc_val:
                # Shift back out to 12 bits (if only the upper bits were sent):
                field_val = field_val << (cls.WD_ADC_BITS - n_upper_adc_bits)

            #print("{}: mask={}, data_int={}, idx_bit_head={}, is_adc_val={}, field_val={}".format(
            #    field_name, hex(field_mask), hex(data_int), idx_bit_head, is_adc_val, field_val
            #))

            named_bitfield_entries[field_name] = field_val
            idx_bit_head += n_bits

        def flip_endianness(name: str, n_bytes: int) -> None:
            """
            Flips the endianness of the field with the given name whose size is 
            `n_bytes` bytes.
            """
            nonlocal named_bitfield_entries
            old_val = named_bitfield_entries[name]
            new_val = int.from_bytes(
                old_val.to_bytes(n_bytes, 'big'), 'little')
            named_bitfield_entries[name] = new_val

        # TODO: Check if flipping is right thing to do for FuelGauge I2C vals (since they're technically a byte array):
        flip_endianness('I2C_BatteryChargeRaw', 2)
        flip_endianness('I2C_BatteryVoltageRaw', 2)
        flip_endianness('I2C_BatteryCurrentRaw', 2)
        flip_endianness('I2C_FuelGaugeTempRaw', 2)

        custom_payload = WatchdogDetailedStatusPacket.CustomPayload(
            **named_bitfield_entries
        )
        return WatchdogDetailedStatusPacket(
            custom_payload=custom_payload,
            pathway=pathway,
            source=source,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        #! TODO (not really a typical use case so not super necessary besides for completeness)
        raise NotImplementedError()

    @ classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        correct_start = len(data) > 0 and data[:1] == cls.START_FLAG
        correct_length = len(data) == 47  # Bytes

        return correct_start and correct_length


class WatchdogHeartbeatPacketInterface(CustomPayloadPacket[CT]):
    # Name of the corresponding prebuilt `Module` used for mapping this packet's
    # data to telemetry streams:
    PREBUILT_MODULE_NAME: str = 'WatchdogHeartbeat'

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    class CustomPayload():
        """
        Core custom WatchdogHeartbeatPacket payload.
        Members must have same names as corresponding telemetry channels in the
        `WatchdogHeartbeat` prebuilt module.
        Note: an error will get thrown by `WatchdogHeartbeatPacket.__init__`
        when building `payloads` from the `custom_payload` if all the channels
        in the `WatchdogHeartbeat` prebuilt module don't have a corresponding
        attr in this `CustomPayload`.


        *NOTE:* For this to work effectively, all fields, including computed
        properties, must match their names from the prebuilt module
        AND the order of the args in `__init__` must match the order of the
        bytes in the packet.
        """

        __slots__: List[str] = [
            '_ChargeRaw',
            '_HeaterStatus',
            '_CurrentRaw',
            '_BatteryVoltageOk',
            '_BattAdcTempRaw'
        ]

        _ChargeRaw: int
        _HeaterStatus: bool
        _CurrentRaw: int
        _BatteryVoltageOk: bool
        _BattAdcTempRaw: int

        @ staticmethod
        def despan(span: int,
                   num_bits: int,
                   min_val: float,
                   max_val: float,
                   span_min: int = 0,
                   span_max: Optional[int] = None
                   ) -> float:
            """
            Converts a given `span` value can span from 0 to 2**`num_bits`
            back to its original value in its original range from `min_val` to
            `max_val`.
            This is just basic linear interpolation over a given bit range.
            Data is transmitted as a `span` value to maximize the number of
            useful values which can occupy a given number of bits.

            Optionally, `span_min` and `span_max` can be given which will limit
            the lower and upper bound value for `span` used in the conversion
            process. `span` values below `span_min` will plateau and correspond to
            `min_val` and `span` values above `span_max` will plateau and
            correspond to `max_val`. This is necessary since, if floating point
            arithmetic is to be avoided in the FSW, it's not always possible to
            map a value from its original range to an exact power of 2.

            **Note:** `max_val` is the value which makes `span` take its highest
            value (2^n-1) and `min_val` is the value which makes `span` 0.
            If the range is inverted (say, 0xFF corresponds to 0 and 0x00
            corresponds to 100), then `max_val` will be smaller than `min_val`.
            """
            span_ub = (2 << (num_bits-1)) - 1  # upper bound of `span` (2^n-1)
            if span_max is None:
                span_max = span_ub
            elif span_max > span_ub:
                raise ValueError(
                    f"Given `span_max` cannot exceed 2**`num_bits`-1={span_ub}. "
                    f"{span_max} was given."
                )

            if span_min < 0:
                raise ValueError(
                    f"Given `span_min` cannot be <0. "
                    f"{span_min} was given."
                )

            # Keep `span` in bounds (and cause it to plateau if out of bounds):
            span = span if span < span_max else span_max
            span = span if span > span_min else span_min

            return (span-span_min)/(span_max-span_min) * (max_val-min_val) + min_val

        @ staticmethod
        def span(val: float,
                 num_bits: int,
                 min_val: float,
                 max_val: float,
                 span_min: int = 0,
                 span_max: Optional[int] = None
                 ) -> float:
            """
            Converts a given value, `val`, which ranges from `min_val` to
            `max_val`, to a `span` value which ranges from 0 to 2**`num_bits`.
            This is just basic linear interpolation over a given bit range.
            Data is transmitted as a `span` value to maximize the number of
            useful values which can occupy a given number of bits.

            Optionally, `span_min` and `span_max` can be given which will limit
            the lower and upper bound value for `span` used in the conversion
            process. `span` values below `span_min` will plateau and correspond to
            `min_val` and `span` values above `span_max` will plateau and
            correspond to `max_val`. This is necessary since, if floating point
            arithmetic is to be avoided in the FSW, it's not always possible to
            map a value from its original range to an exact power of 2.

            **Note:** `max_val` is the value which makes `span` take its highest
            value (2^n-1) and `min_val` is the value which makes `span` 0.
            If the range is inverted (say, 0xFF corresponds to 0 and 0x00
            corresponds to 100), then `max_val` will be smaller than `min_val`.
            """
            span_ub = (2 << (num_bits-1)) - 1  # upper bound of `span` (2^n-1)
            if span_max is None:
                span_max = span_ub
            elif span_max > span_ub:
                raise ValueError(
                    f"Given `span_max` cannot exceed 2**`num_bits`-1={span_ub}. "
                    f"{span_max} was given."
                )

            if span_min < 0:
                raise ValueError(
                    f"Given `span_min` cannot be <0. "
                    f"{span_min} was given."
                )

            # Keep `val` in bounds (and cause it to plateau if out of bounds):
            val = val if val < max_val else max_val
            val = val if val > min_val else min_val

            return (val-min_val)/(max_val-min_val) * (span_max-span_min) + span_min

        @property
        def _module(self) -> Module:
            return WatchdogHeartbeatPacketInterface.get_ds_module()

        @property
        def ChargeMah(self) -> float:
            return self.despan(self._ChargeRaw, 7, 29.1, 3500, span_max=120)

        @property
        def ChargePercent(self) -> float:
            return self.ChargeMah / 3500 * 100

        @property
        def CurrentMilliamps(self) -> float:
            return self.despan(self._CurrentRaw, 7, 0, 600, span_max=120)

        @property
        def BattAdcTempKelvin(self) -> float:
            return self.despan(self._BattAdcTempRaw, 8, 75, -12.31, span_max=233)

        # Make public get, private set to signal that you can freely use these values
        # but modifying them directly can yield undefined behavior (specifically
        # `raw` not syncing up with whatever other data is in the container)

        @property
        def ChargeRaw(self) -> int: return self._ChargeRaw
        @property
        def HeaterStatus(self) -> bool: return self._HeaterStatus
        @property
        def CurrentRaw(self) -> int: return self._CurrentRaw
        @property
        def BatteryVoltageOk(self) -> bool: return self._BatteryVoltageOk
        @property
        def BattAdcTempRaw(self) -> int: return self._BattAdcTempRaw

        def __init__(self,
                     ChargeRaw: int,
                     HeaterStatus: int,
                     CurrentRaw: int,
                     BatteryVoltageOk: int,
                     BattAdcTempRaw: int
                     ) -> None:
            self._ChargeRaw = ChargeRaw
            self._HeaterStatus = bool(HeaterStatus)
            self._CurrentRaw = CurrentRaw
            self._BatteryVoltageOk = bool(BatteryVoltageOk)
            self._BattAdcTempRaw = BattAdcTempRaw

        def __repr__(self) -> str:
            return (
                f"[HEAT]: {' ON' if self.HeaterStatus else 'OFF'} \t\t "
                f"[BATT]: Voltage: {'GOOD' if self.BatteryVoltageOk else 'BAD'}, "
                f"Charge: {self.ChargeMah:4.0f}mAh = {self.ChargePercent:5.1f}%, "
                f"Temp: {self.BattAdcTempKelvin:5.1f}°K]"
            )


class WatchdogHeartbeatPacket(WatchdogHeartbeatPacketInterface[WatchdogHeartbeatPacketInterface]):
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\xFF'  # Required start flag

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    def __repr__(self) -> str:
        return self.custom_payload.__repr__()

    @ classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE
               ) -> WatchdogHeartbeatPacket:
        flag, core_data = data[:1], data[1:]
        if cls.START_FLAG != flag:
            raise PacketDecodingException(
                data,
                "Start flag for `WatchdogTvacHeartbeatPacket` was invalid. "  # type: ignore
                f"Expected {cls.START_FLAG}, Got: {flag} ."
            )
        # TODO: use `bitstruct.compile` on first run to speed processing time.
        custom_payload = WatchdogHeartbeatPacket.CustomPayload(
            *bitstruct.unpack('u7u1u7u1u8', core_data)
        )
        return WatchdogHeartbeatPacket(
            custom_payload=custom_payload,
            pathway=pathway,
            source=source,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        #! TODO (not really a typical use case so not super necessary besides for completeness)
        raise NotImplementedError()

    @ classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[:1] == cls.START_FLAG
        right_length = len(data) == 4  # Bytes

        return right_start and right_length


class WatchdogCommandResponsePacketInterface(CustomPayloadPacket[CT]):
    # Name of the corresponding prebuilt `Module` used for mapping this packet's
    # data to telemetry streams:
    PREBUILT_MODULE_NAME: str = 'WatchdogCommandResponse'

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
        bytes in the packet.
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


class WatchdogCommandResponsePacket(WatchdogCommandResponsePacketInterface[WatchdogCommandResponsePacketInterface]):
    START_FLAG: bytes = b'\x0A'  # Required start flag

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    def __repr__(self) -> str:
        return self.custom_payload.__repr__()

    @ classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE,
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
        return WatchdogCommandResponsePacket(
            custom_payload=custom_payload,
            pathway=pathway,
            source=source,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        #! TODO (not really a typical use case so not super necessary besides for completeness)
        raise NotImplementedError()

    @ classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[:1] == cls.START_FLAG
        right_length = len(data) == 3  # Bytes

        return right_start and right_length

class WatchdogTvacHeartbeatPacket(WatchdogTvacHeartbeatPacketInterface[WatchdogTvacHeartbeatPacketInterface]):
    # Properties (r-only class variables):
    START_FLAG: bytes = b'\xFF'  # Required start flag

    # Empty __slots__ allows super's __slots__ to not turn into __dict__:
    __slots__: List[str] = []

    def __repr__(self) -> str:
        return self.custom_payload.__repr__()

    @ classmethod
    def decode(cls,
               data: bytes,
               endianness_code: str = ENDIANNESS_CODE,
               pathway: DataPathway = DataPathway.NONE,
               source: DataSource = DataSource.NONE
               ) -> WatchdogTvacHeartbeatPacket:
        flag, core_data = data[:1], data[1:]
        if cls.START_FLAG != flag:
            raise PacketDecodingException(
                data,
                "Start flag for `WatchdogTvacHeartbeatPacket` was invalid. "  # type: ignore
                f"Expected {cls.START_FLAG}, Got: {flag} ."
            )
        custom_payload = WatchdogTvacHeartbeatPacket.CustomPayload(
            *struct.unpack(endianness_code + '9H 3B H', core_data)
        )
        return WatchdogTvacHeartbeatPacket(
            custom_payload=custom_payload,
            pathway=pathway,
            source=source,
            raw=data,
            endianness_code=endianness_code
        )

    def encode(self, **kwargs: Any) -> bytes:
        #! TODO (not really a typical use case so not super necessary besides for completeness)
        raise NotImplementedError()

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[:1] == cls.START_FLAG
        right_length = len(data) == 24  # Bytes

        return right_start and right_length