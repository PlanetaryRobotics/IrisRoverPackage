#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Loads and Stores Collection of Standard Definitions for All Uplinked and 
Downlinked Data in accordance with the Command & Telemetry List (C&TL)

@author: Connor W. Colombo (CMU)
@last-updated: 12/28/2020
"""

from __future__ import annotations # Activate postponed annotations (for using classes as return type in their own methods)

from typing import List, Optional, Dict, Any, TypeVar, Generic, Union, Tuple, MutableMapping, Iterator

from dataclasses import dataclass
from enum import Enum

import os.path
from lxml import etree
import json

class FswDataType(Enum):
    """Enum for Respresenting FPrime Datatypes as Python Struct Strings."""
    
    BOOL = '?'
    I8 = 'b'
    I16 = 'h'
    I32 = 'l'
    I64 = 'q'
    U8 = 'B'
    U16 = 'H'
    U32 = 'L'
    U64 = 'Q'
    F32 = 'f' # IEEE 754 binary32 float
    F64 = 'd' # IEEE 754 binary64 double
    

VT = TypeVar('VT')

class NameIdDict(MutableMapping[Union[int,str],VT]):
    """
    A dictionary where each value is keyed by both a string name and numeric id.
    
    Used for looking up modules, commands, etc. by either name or opcode/id.
    """
    id_dict: Dict[int, VT]
    name_id_mapping: Dict[str, int]
    
    def __getitem__(self, key: Union[int, str], value: VT) -> VT:
        if isinstance(key, int):
            return self.id_dict[key]
        if isinstance(key, str):
            return self.id_dict[name_id_mapping[key]]
    
    def __setitem__(self, key: Union[int, str, Tuple[int,str]], value: VT) -> None:
        if isinstance(key, int) or isinstance(key, str):
            raise TypeError("NameIdDict should only have values set with a Tuple[int,str]")
        if isinstance(key, Tuple[int,str]):
            self.id_dict[key[0]] = value
            self.name_id_mapping[key[1]] = key[0]
            
    def __delitem__(self, key: Union[int, str]) -> None:
        if isinstance(key, int):
            # Ensure key is in dictionary and remove it:
            if key in self.id_dict:
                del self.id_dict[key]
            # Find all names which map to this ID key (should only be 1 name 
            # except maybe in a weird edge case where type checking is ignored) 
            # and remove them:
            for k,v in self.name_id_mapping.items():
                if v == key:
                    del self.name_id_mapping[k]
        if isinstance(key, str):
            # Ensure key is in names mapping and remove it and it's associated 
            # ID (invoke __delitem__ to do so to handle any extra cleanup required).
            if key in self.name_id_mapping:
                ID = self.name_id_mapping[key]
                del self.name_id_mapping[key]
                self.__delitem__(key = ID)

    def __len__(self) -> int:
        return len(self.id_dict)
    
    def __iter__(self) -> Iterator[]:
        return iter()
        ## TODO: __iter__ and __next__ ? Iterator should be like enumerate with ((int,str), VT) return type
        ## May have to update __len__ so it matches number of Iterator outputs? (if start mapping with name_id_mapping)
        ## Then done with class

class DataStandards(object):
    """Loads and Stores the Standard Definitions for All Uplinked and Downlinked Data."""
    
    class Module(object):
        """Container for Data Standards for a Module in the FSW."""
        
        __slots__:List[str] = [
            'name', #       - Name of the Module
            'ID', #         - Highest 1B of the Command Opcodes for this Module (its ID)
            'commands', #   - Map of Module Commands
            'telemetry' #   - Map of Module Telemetry Channels
        ]
        
        class Command(object):
            """Container for Data Standards for a Command in the FSW."""
            
            @dataclass(order=False)
            class Argument(object):
                """Container for FSW Command Argument Metadata."""
                
                __slots__ = ['fprime_name', 'datatype']
                fprime_name: str
                datatype: FswDataType
            
            __slots__:List[str] = [
                'name', #       - Name of the Command
                'mnemonic', #   - FPrime Command Mnemonic
                'ID', #         - Lowest 1B of Command Opcode
                'args', #       - List of FSW Command Arguments
                'metadata_json_str' #   - GSW-specific Metadata JSON String (from FPrime XML Comment)
                '__metadata_dict' #     - GSW-specific Metadata Dictionary (from JSON String)
            ]
            
            __metadata_dict: Dict[Any,Any]
            
            def __init__(self, # Command
                name: str,
                mnemonic: str,
                ID: int,
                metadata_json_str: str,
                args: List[DataStandards.Module.Command.Argument] = None
            ) -> None:
                self.name = name
                self.mnemonic = mnemonic
                self.ID = ID
                self.metadata_json_str = metadata_json_str
                
                self.extract_metadata(self.metadata_json_str)
                
                if args is None:
                    self.args = []
                else:
                    self.args = args
            
            @property
            def metadata(self) -> Dict[Any,Any]:
                """Return cached conversion of metadata json string to a dictionary."""
                return self.__metadata_dict
            
            def extract_metadata(self, json_str: str) -> None:
                """Reconvert metadata json string to a dictionary and cache."""
                self.__metadata_dict = json.loads(self.metadata_json_str)
                
        
        def __init__(self, # Module
            name: str,
            ID: int,
            commands: Dict[int, Command],
            commands_by_ui_name: Dict[str, Command]
            telemetry: Dict[int, Telemetry]
        ) -> None:
            self.name = name
            self.ID = ID
            self.commands = commands
            self.telemetry = telemetry
            
            
    __slots__:List[str] = [
        'modules' #     - Modules indexed by opcode (highest 1B)
    ]
    
    def __init__(self, # DataStandards
        modules: Dict[int, Module]
    ) -> None:
        self.modules = modules
        pass
    
    def cache(self,
        cache_dir:str = './DataStandardsCache'
    ) -> str:
        """
        Cache this DataStandards instance in a unique file in `cache_dir`.
       
        Returns the unique filename.
        """
        #### TODO
        pass
    
    @classmethod
    def load_standards_cache(cls,
        cache_dir: str = './DataStandardsCache',
        cache_name: Optional[str] = None
    ) -> DataStandards:
        """
        Create a new DataStandards instance by loading a cached file from `cache_dir`.
        
        The loaded file will be the latest cache in `cache_dir` unless the 
        name of specific cache file is supplied with `cache_name`.
        """
        #### TODO
        pass
    
    @classmethod
    def build_standards(cls,
        search_dir: str = '../../../FlightSoftware/fprime/CubeRover',
        cache_dir: str = './DataStandardsCache'
    ) -> DataStandards:
        """
        Create a new DataStandards instance by indexing the relevant standard definitions.
        
        Standard definition files are held in `search_dir`. The sole source of 
        truth for which files to index and how to process them lies in this 
        function and its subroutines.
        
        Once built, the finalized DataStandards instance is cached in 
        `cache_dir` without overriding previous caches.
        """
        ## Settings:
        # Relative Locations of Import Definition Files and Directories w.r.t. `search_dir`:
        uri_topology = './Top/CubeRoverTopologyAppAi.xml'
        
        # Important Tree XPath Selectors:
        xpath_modules = """/assembly/instance[@namespace="CubeRover"]"""
        
        ## Load Important Trees:
        tree_topology = etree.parse(os.path.join(search_dir, uri_topology))
        
        #### TODO
        pass
        
    