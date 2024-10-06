Module IrisBackendv3.data_standards.fsw_data_type
=================================================
Enum for Representing FPrime Datatypes as Python Struct Strings.

@author: Connor W. Colombo (CMU)
@last-updated: 11/12/2022

Classes
-------

`Category(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Various categories for serialization of FSW Types, which define how they
    will be en/decoded in the codec layer. Each category has its own way of
    serializing values.

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `BOOLEAN`
    :

    `EMPTY`
    :

    `ENUM`
    :

    `IRISBYTESTRING`
    :

    `NUMBER`
    :

    `STRING`
    :

    `VARSTRING`
    :

`FswDataType(value, names=None, *, module=None, qualname=None, type=None, start=1)`
:   Enum for Representing Native FPrime Datatypes.
    
    Each instance contains four explicit auxiliary data fields: `struct_sym`,
    `type_str`, and `category`.
    - `struct_sym` is the symbol used for representing this datatype when using
    `struct.pack`
    - `type_str` is the standard embedded (C-style) notation of writing this
    data type (sans the `_t` at the end).
    - `category` is the FSW serialization category which is used to determine
    how to en/decode a value with this type.
    -  `python_type` is the corresponding type or tuple of types in python (eg. 
    `str`, `(int,str)`) which a value must have in order to be able to be 
    accurately encoded as this FswDataType.
    
    `type_str` is used as the default `_value_` for each instance. Thus,
    `FswDataType('uint8') == FswDataType['U8']`. This makes fetching the right
    instance easier since one can go from either useful direction (FPrime unit
    type or standard type name).
    
    Additionally, there's one optional field `num_octets` which gives the number
    of 8b bytes (octets) in this type's size. This is normally straightforward
    and can be computed using `struct.calcsize(struct_sym)` but if for some
    reason it's more complex or needs to be explicit, this field can be
    specified too.
    
    NOTE: For variable length types, `num_octets` is the MAXIMUM size of the
    datatype when encoded.

    ### Ancestors (in MRO)

    * enum.Enum

    ### Class variables

    `BOOL`
    :

    `BOOLEAN`
    :

    `ENUM`
    :

    `F32`
    :

    `F64`
    :

    `I16`
    :

    `I32`
    :

    `I64`
    :

    `I8`
    :

    `INVALID`
    :

    `IRISBYTESTRING134`
    :

    `STRING10`
    :

    `STRING100`
    :

    `STRING15`
    :

    `STRING24`
    :

    `STRING240`
    :

    `STRING39`
    :

    `STRING40`
    :

    `STRING5`
    :

    `STRING50`
    :

    `STRING6`
    :

    `STRING8`
    :

    `U16`
    :

    `U32`
    :

    `U64`
    :

    `U8`
    :

    `VARSTRING_10K`
    :

    `VARSTRING_255`
    :

    `category: IrisBackendv3.data_standards.fsw_data_type.Category`
    :

    `num_octets: int`
    :

    `python_type: Union[type, Tuple[type]]`
    :

    `struct_sym: str`
    :

    `type_str: str`
    :

    ### Static methods

    `get(name: str) ‑> IrisBackendv3.data_standards.fsw_data_type.FswDataType`
    :   Gets the `FswDataType` enum member which matches to the given name if 
        one exists. Matching is done by:
        - Enum member name (the FPrime name), once UPPERCASED.
        - Enum member value (the C-style embedded name), once any '_t' are removed.
        
        Returns:
            FswDataType: Matched member.
        
        Raises:
            KeyError: if the name is not valid / doesn't match to any members.

    `is_valid_C_name(name: str) ‑> bool`
    :   Tests if the given name is a valid FswDataType value(C-style type name).
        If it is, you can get the associated enum instance with `FswDataType(name)`.

    `is_valid_FPrime_name(name: str) ‑> bool`
    :   Tests if the given name is a valid FswDataType name(FPrime type name).
        If it is, you can get the associated enum instance with `FswDataType[name]`.

    ### Instance variables

    `num_bits: int`
    :   Number of bits in the datatype (or max number in the case of a
        variable-length string).

    ### Methods

    `get_actual_num_bytes(self, data: Optional[bytes] = None) ‑> int`
    :   Returns the actual number of bytes in this datatype.
        For most types, this is a fixed length determined just by the type;
        however, for a variable-length string it's given by the first two bytes
        in the data.
        
        @param data: The data that has this type (only needed for variable
        length strings).

    `get_max_num_bytes(self) ‑> int`
    :   Returns the maximum number of bytes for an encoded representation of
        the datatype.