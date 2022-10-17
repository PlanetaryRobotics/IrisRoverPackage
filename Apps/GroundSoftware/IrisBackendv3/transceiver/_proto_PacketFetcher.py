#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 20 12:34:45 2020.

@author: connorcolombo
"""

from Packet import Packet

from typing import List
from collections.abc import Iterator
from abc import abstractmethod

class PacketFetcher(Iterator):
    """
    Interface for retrieving packets from an arbitrary data stream.
    
    @author: Connor W. Colombo (CMU)
    """
    
    __slots__:List[str] = []
    
    def __init__(self) -> None:
        self.initialize_stream()
        
    def __iter__(self) -> Iterator[Packet]:
        """Set up Standard Iterator."""
        return self
        
    def __next__(self) -> Packet:
        """Get Next Packet Bytes for Iterator."""
        return self.get_next_packet()
    
    @abstractmethod
    def initialize_stream(self) -> None:
        """Initialize the stream used to fetch the packets."""
        pass
    
    @abstractmethod
    def get_next_packet(self) -> Packet:
        """Fetch and clean the next packet from the stream."""
        pass