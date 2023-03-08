Module IrisBackendv3.codec.exceptions
=====================================
Various exceptions used throughout this package.

@author: Connor W. Colombo (CMU)
@last-updated: 04/09/2021

Classes
-------

`PacketDecodingException(data: bytes, info: str = '')`
:   An exception occurred while decoding a packet.
    
    Create a PacketDecodingException.
    
    Args:
        data (bytes): Data which couldn't be decoded properly and caused the exception.
        info (str, optional): Info about the specific formatting issue. Defaults to "".

    ### Ancestors (in MRO)

    * builtins.Exception
    * builtins.BaseException

`PacketEncodingException(data: Any, info: str = '')`
:   An exception occurred while encoding a packet.
    
    Create a PacketEncodingException.
    
    Args:
        data (bytes): Data which couldn't be encoded properly and caused the exception.
        info (str, optional): Info about the specific formatting issue. Defaults to "".

    ### Ancestors (in MRO)

    * builtins.Exception
    * builtins.BaseException