#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Defines Common Data Required for Packets. Support for Building and Parsing 
Packets.

@author: Connor W. Colombo (CMU)
@last-updated: 12/26/2020
"""
from __future__ import annotations # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List, Tuple
from abc import ABC
import struct

class IrisPacket(ABC):
    """
    Defines Common Data Required for All Packets.
    
    Packets follow the standard defined in the IRIS Command & Telemetry List.
    
    All data is stored in the unpacked form since that's how it will most often 
    be handled and transformed and only packed into bytes when needed.
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 12/26/2020
    """
    
    class CommonPacketHeader(object):
        """
        Container for Handling and Processing CommonPacketHeaders.
        
        All data is stored in the unpacked form since that's how it will most 
        often be handled and transformed and only packed into bytes when needed.
        """
        
        __slots__:List[str] = [
            'endianness_code', #    - Python Struct Endianness Code
            'seq_num', #            - Sequence Number
            'vlp_len', #            - Variable Length Payload (VLP) Length
            'checksum' #            - Checksum of Entire Variable Length Payload (VLP)
            '__weakref__'
        ]
        
        def __init__(self,
            seq_num: int,
            vlp_len: int,
            checksum: bytes,
            endianness_code: str = "!" # network order by default
        ) -> None:
            self.seq_num = seq_num
            self.vlp_len = vlp_len
            self.checksum = checksum
            self.endianness_code = endianness_code
        
        @staticmethod
        def format_string(endianness_code: str) -> str:
            """Generate the struct format string for a CPH with the given endianness."""
            return endianness_code + ' B H 2s' # SeqNum (1B) + Length (2B) + Checksum (2B)
        
        @classmethod
        def unpack(cls, endianness_code: str, data: bytes) -> IrisPacket.CommonPacketHeader:
            """Extract all data in the given raw common packet header."""
            seq_num, vlp_len, checksum = struct.unpack(IrisPacket.CommonPacketHeader.format_string(endianness_code), data)
            return cls(
                endianness_code = endianness_code,
                seq_num = seq_num,
                vlp_len = vlp_len,
                checksum = checksum
            )
        
        @classmethod
        def from_bytes(cls, endianness_code: str, data: bytes) -> IrisPacket.CommonPacketHeader:
            """Construct a CPH Object from Bytes. Alias for unpack."""
            return cls.unpack(endianness_code, data)
        
        def pack(self) -> bytes:
            """Pack data into a bytes object."""
            return struct.pack(
                IrisPacket.CommonPacketHeader.format_string(self.endianness_code),
                self.seq_num,
                self.vlp_len,
                self.checksum
            )
            
        @property
        def raw_data(self) -> bytes:
            """Return the raw bytes. Alias for pack."""
            return self.pack();
        
    magics = {
        'command': 0x00BADA55, #bytes([0x00,0xBA,0xDA,0x55]),
        'watchdog_command': 0xC000FFEE, #bytes([0xC0,0x00,0xFF,0xEE]),
        'telemetry': 0xC00010FF, #bytes([0xC0,0x00,0x10,0xFF]),
        'log': 0x0DEADBAD, #bytes([0x0D,0xEA,0xDB,0xAD]),
        'file': 0xDABAD000, #bytes([0xDA,0xBA,0xD0,0x00]),
    }
    
    __slots__:List[str] = [
        'common_packet_header',
        'variable_length_payload'
    ]
    common_packet_header: CommonPacketHeader
    variable_length_payload: List[Tuple[bytes,bytes]]
    
    def __init__(self,
         endianness_code
    ) -> None:
        pass
    
    def __len__(self) -> int:
        pass
        
    @classmethod
    def from_bytes(cls, endianness_code: str, data: bytes) -> CommonPacketHeader:
        """Construct a Iris Packet Object from Bytes."""
        # Remove the first 5 bytes as the CPH (SeqNum (1B) + Length (2B) + Checksum (2B)):
        CPH = IrisPacket.CommonPacketHeader.from_bytes(endianness_code, data[:5])
        
        # Unpack Variable Length Payload:
        
        pass
    
    @property
    def CPH(self) -> CommonPacketHeader:
        """Alias for common_packet_header."""
        return self.common_packet_header
    
    @property
    def raw_data(self) -> bytes:
        self.CPH
        pass
    
    
    #class VLP(Iterator) # allow iterating through the VLP (payloads)
        
    
class IrisDownlinkPacket(IrisPacket):
    """
    Defines Common Data Required for All Downlinked Packets (Moon to Earth).
    
    All the same core data as any other IrisPacket but with special additional 
    metadata coming from 
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 12/25/2020
    """
    
    __slots__:List[str] = [
        'time-received',
        'time-sent'
    ]
    
    
    def __init__(self) -> None:
        pass
    
class IrisUplinkPacket(Packet):
    """
    Defines Common Data Required for All Uplinked Packets (Earth to Moon).
    
    @author: Connor W. Colombo (CMU)
    @last-updated: 12/25/2020
    """
    
    __slots__:List[str] = [
        'time-issued',
        'time-logged',
        'time-processed',
        'time-sent',
        'time-confirmed',
        'time-received'
    ]

    
    def __init__(self) -> None:
        pass
    
    