#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Defines Common Data Required for Packets. Support for Building and Parsing
Packets.

@author: Connor W. Colombo (CMU)
@last-updated: 05/06/2020
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List, Tuple, Any, Optional, Callable, Generic, TypeVar, cast, Union, Generic
from abc import ABC, abstractmethod
from enum import Enum
import traceback

import struct
import bitstruct  # type: ignore
import numpy as np
import time
from math import ceil

from .magic import Magic, MAGIC_SIZE
from .metadata import DataPathway, DataSource
from .container import ContainerCodec
from .payload import PayloadCollection, TelemetryPayload, extract_downlinked_payloads

from .settings import ENDIANNESS_CODE, settings
from .logging import logger
from .exceptions import PacketDecodingException

from IrisBackendv3.utils.basic import print_bytearray_hex as printraw
from IrisBackendv3.data_standards.module import Module

CPH_SIZE = 4

CT = TypeVar('CT')

# TODO: Add update hooks?

# TODO: Add `__str__` / `__repr__`s

#! TODO: Handle serialization (must replace container scheme, augment by storing payloads with their metadata)


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
                byte_order=ENDIANNESS_CODE
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

        @staticmethod
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

        @staticmethod
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

    @classmethod
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
                f"> Command #{self._CommandId} responded with {self.ErrorFlagName}[{hex(self.ErrorFlag)}]: '{self.ErrorFlagComment}'."
            )

        def __repr__(self) -> str:
            return (
                f"> Command[#{self._CommandId}] -> {self.ErrorFlagName}[{hex(self.ErrorFlag)}]"
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

    @classmethod
    def is_valid(cls, data: bytes, endianness_code: str = ENDIANNESS_CODE) -> bool:
        """
        Determines whether the given bytes constitute a valid packet of this type.
        """
        right_start = len(data) > 0 and data[:1] == cls.START_FLAG
        right_length = len(data) == 3  # Bytes

        return right_start and right_length
