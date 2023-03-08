Module IrisBackendv3.codec.fsw_data_codec
=========================================
Enum for En/Decoding FPrime Datatypes using Python Struct Strings.

@author: Connor W. Colombo (CMU)
@last-updated: 11/12/2022

Functions
---------

    
`category_to_codec(datatype: FswDataType) ‑> IrisBackendv3.codec.fsw_data_codec.Codec`
:   Returns the appropriate Codec for the given FswDataType.

    
`decode(datatype: FswDataType, buffer: bytes) ‑> Any`
:   Decodes the given data which has the given FswDataType.
    
    Given buffer must only contain data that should be part of this field
    (and not, for example, the entire rest of the message).
    
    (A simple external access method.)

    
`encode(datatype: FswDataType, data: Any) ‑> bytes`
:   Encodes the given data which has the given FswDataType.
    (A simple external access method.)

    
`format_string(datatype: FswDataType, data: Optional[bytes] = None) ‑> str`
:   Creates the appropriate format string for the given `FswDataType`.
    
    This format string only depends on the data in the case of a Variable
    length string, in which case the data needs to be supplied.

Classes
-------

`BooleanPacker()`
:   Defines how to en/decode a Boolean.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.fsw_data_codec.Codec
    * typing.Generic
    * abc.ABC

`BytesPacker()`
:   Defines how to en/decode a string of arbitrary raw bytes for FSW (i.e.
    impl. of `Fw::IrisCmdByteStringArg` from FSW).
    *Note:* Here the *args must be [max_length, bytes]

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.fsw_data_codec.Codec
    * typing.Generic
    * abc.ABC

    ### Static methods

    `check(format_string: str, args: Tuple[int, Union[str, bytes]]) ‑> bool`
    :   Checks the if the given vals can be used to encode as a Byte String.
        where *args are [max_length, string]

    `decode(format_string: str, buffer: bytes) ‑> Tuple[int, bytes]`
    :   Decodes the given val as `bytes`.

    `encode(format_string: str, args: Tuple[int, bytes]) ‑> bytes`
    :   Encodes the given val as a Byte String.
        where *args are [max_string_length, string].
        NOTE: Format string will contain the ACTUAL number of bytes to use for
        the encoding (and it will clip if more data is supplied).

`Codec()`
:   Generic ABC detailing the methods which all type codecs for `FSWDataType`s
    should have.

    ### Ancestors (in MRO)

    * typing.Generic
    * abc.ABC

    ### Descendants

    * IrisBackendv3.codec.fsw_data_codec.BooleanPacker
    * IrisBackendv3.codec.fsw_data_codec.BytesPacker
    * IrisBackendv3.codec.fsw_data_codec.GenericCodec
    * IrisBackendv3.codec.fsw_data_codec.NumberPacker
    * IrisBackendv3.codec.fsw_data_codec.StringPacker

    ### Static methods

    `check(format_string: str, data: CT) ‑> bool`
    :   Checks whether the given value(s) is able to be encoded as this type.

    `decode(format_string: str, buffer: bytes) ‑> ~CT`
    :   Decodes the given bytes buffer as the given type and returns a tuple
        containing the extracted values.

    `encode(format_string: str, data: CT) ‑> bytes`
    :   Encodes the given value(s) as the given type and returns the bytes.

`GenericCodec()`
:   Defines how to code a generic object which should un/packed exactly as
    `struct` would un/pack it.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.fsw_data_codec.Codec
    * typing.Generic
    * abc.ABC

`NumberPacker()`
:   Defines how to en/decode a Numeric Type.

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.fsw_data_codec.Codec
    * typing.Generic
    * abc.ABC

`StringPacker()`
:   Defines how to en/decode a String with a Variable-Length up to a fixed
    limit. Think of this length limit as the amount of buffer space allocated
    in the FSW.
    *Note:* Here the *args must be [max_encoded_length, string]

    ### Ancestors (in MRO)

    * IrisBackendv3.codec.fsw_data_codec.Codec
    * typing.Generic
    * abc.ABC

    ### Static methods

    `check(format_string: str, args: Tuple[int, Union[str, bytes]]) ‑> bool`
    :   Checks the if the given vals can be used to encode as a Fixed Length String.
        where *args are [fixed_length, string]

    `decode(format_string: str, buffer: bytes) ‑> Tuple[int, str]`
    :   Decodes the given val as a String.

    `decode_str(raw_data: bytes) ‑> str`
    :

    `encode(format_string: str, args: Tuple[int, str]) ‑> bytes`
    :   Encodes the given val as a String.
        where *args are [max_encoded_length, string]
        NOTE: Format string will contain the ACTUAL number of bytes to use for
        the encoding (and it will clip if more data is supplied).

    `encode_str(data: str) ‑> bytes`
    :