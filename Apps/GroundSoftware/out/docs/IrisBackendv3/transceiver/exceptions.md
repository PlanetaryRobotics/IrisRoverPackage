Module IrisBackendv3.transceiver.exceptions
===========================================
Various exceptions used throughout this package.

@author: Connor W. Colombo (CMU)
@last-updated: 05/10/2022

Classes
-------

`TransceiverConnectionException(info: str = '')`
:   A `Transceiver` failed to connect to an uplink/downlink channel.
    
    Create a TransceiverConnectionException.
    
    Args:
        info (str, optional): Info about the specific formatting issue. Defaults to "".

    ### Ancestors (in MRO)

    * builtins.Exception
    * builtins.BaseException

`TransceiverDecodingException(data: bytes, info: str = '')`
:   An exception occurred while decoding a packet.
    
    Create a TransceiverDecodingException.
    
    Args:
        data (bytes): Data which couldn't be decoded properly and caused the exception.
        info (str, optional): Info about the specific formatting issue. Defaults to "".

    ### Ancestors (in MRO)

    * builtins.Exception
    * builtins.BaseException

`TransceiverEncodingException(data: Any, info: str = '')`
:   An exception occurred while encoding a packet.
    
    Create a TransceiverEncodingException.
    
    Args:
        data (bytes): Data which couldn't be encoded properly and caused the exception.
        info (str, optional): Info about the specific formatting issue. Defaults to "".

    ### Ancestors (in MRO)

    * builtins.Exception
    * builtins.BaseException