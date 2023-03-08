Module IrisBackendv3.utils.basic
================================
A Collection of Basic Utility Functions.

@author: Connor W. Colombo (CMU)
@last-updated: 09/17/2022

Functions
---------

    
`byte_to_binstr(B)`
:   

    
`bytearray_to_spaced_hex(ba, spacer: str = ' ') ‑> str`
:   

    
`bytes_to_binstr(Bs)`
:   

    
`dict_field_type_check(data: Dict, field: str, desired_type: Union[type, List[type]], name: str = None) ‑> None`
:   Check that the given field of a dict has the desired type. If not, raise a TypeError.
    
    Optionally, a `name` can be provided which matches the variable name of the
    given `data` dict in the calling environment and will be displayed in the
    event of a type error.
    
    `desired_type` can also be a list of types, only one of which need to match.

    
`dict_type_check(data: Dict, type_dict: Dict[str, Union[type, List[type]]], name: str = None) ‑> None`
:   Check that all fields in the given dictionary have the desired types. If not, raise a TypeError.
    
    Desired types are given in a type dictionary where each key matches a key in
    the data dictionary being evaluated, and each value is the desired type.
    
    Each of the specified type values for each key can also be a list of types,
    only one of which need to be matched.
    
    Optionally, a `name` can be provided which matches the variable name of the
    given `data` dict in the calling environment and will be displayed in the
    event of a type error.
    
    Example:
    To make sure the `name` field in the given `data` dictionary is a `str` and
    the `ID` field is an `int` or `float`, the following `type_dict` must be given:
    type_dict = {'name': str, 'ID': [int, float]}

    
`flip_all_bits_in_bytes(bs: bytes) ‑> bytes`
:   Reverses the order of all bits in the bytes object
    
    # Examples:
    >>> bytes_to_binstr(flip_all_bits_in_bytes(0b111111101110110111011110101011011011111011101111.to_bytes(6,'big')))
    '0b 11110111 01111101 10110101 01111011 10110111 01111111'

    
`flip_bits_in_byte(one_byte: Union[int, bytes]) ‑> Union[int, bytes]`
:   Reverses the order of all bits in the given byte (as an int or a 1 byte long bytes object).

    
`full_dict_spec_check(data: Dict, spec_dict: Dict[str, Union[type, List[type]]], name: str = None) ‑> None`
:   Check that the given `data` dictionary has, at least, all the fields
    specified in keys of `spec_dict` and that all of those fields have the
    types specified by the values of `spec_dict`. If not a KeyError or
    TypeError is raised as appropriate.
    
    Each of the specified type values for each key can also be a list of types,
    only one of which need to be matched.
    
    Optionally, a `name` can be provided which matches the variable name of the
    given `data` dict in the calling environment and will be displayed in the
    event of a type error.
    
    Example:
    To make sure the given `data` dict contains the keys `name` and `field` and
    the that name` field in the given `data` dictionary is a `str` and the `ID`
    field is an `int` or `float`, the following `spec_dict` must be given:
    spec_dict = {'name': str, 'ID': [int, float]}

    
`hexstr_to_bytes(hexstr: str, delimiter: str = ' ')`
:   Converts the given hex string, delimited with the given delimiter, to a
    bytes object.
    
    # Examples:
    >>> hexstr_to_bytes('52 41 44 3A 45 43 48 4F 7C 68 48 45 4C 4C 4F', ' ')
    b'RAD:ECHO|hHELLO'

    
`name_split_and_format(name: str) ‑> List[str]`
:   Split a name string by all typical delimiters and return its pieces with consistent formatting.
    
    # Details:
    Given a name string where components of the name could be delimited by
    certain special characters ('_', '-', ':', or whitespace) or capitalization,
    split on all possible boundaries and the format each piece by ensuring that
    only the first letter is capitalized.
    
    Returns a list of all the pieces.
    
    # Examples:
    # Trivial:
    >>> name_split_and_format('hello')
    ['Hello']
    >>> name_split_and_format('Hello')
    ['Hello']
    >>> name_split_and_format('101Dalmatians')
    ['101', 'Dalmatians']
    
    # Split on Any number of Special Characters:
    >>> name_split_and_format('hello world')
    ['Hello', 'World']
    >>> name_split_and_format('hello      world')
    ['Hello', 'World']
    >>> name_split_and_format('hello-world')
    ['Hello', 'World']
    >>> name_split_and_format('hello--world')
    ['Hello', 'World']
    >>> name_split_and_format('hello::world')
    ['Hello', 'World']
    >>> name_split_and_format('hello__world')
    ['Hello', 'World']
    
    # Split on Capitalization:
    >>> name_split_and_format('helloWorld')
    ['Hello', 'World']
    
    # With other characters:
    >>> name_split_and_format('hello55World99')
    ['Hello55', 'World99']
    
    # With other adjacent digits:
    >>> name_split_and_format('hello55World99-2')
    ['Hello55', 'World99', '2']
    
    # With other adjacent digits and decimals:
    >>> name_split_and_format('hello55World99-2.0')
    ['Hello55', 'World99', '2.0']
    
    # With numbers in the middle:
    >>> name_split_and_format('hello55world')
    ['Hello55', 'World']
    
    # With symbols in the middle:
    >>> name_split_and_format('hello(world)')
    ['Hello', '(', 'World', ')']
    
    # With Initialisms:
    >>> name_split_and_format('RSSI_Data')
    ['R', 'S', 'S', 'I', 'Data']
    
    # Combination:
    >>> name_split_and_format('FSW::say_hello55World99')
    ['F', 'S', 'W', 'Say', 'Hello55', 'World99']

    
`name_split_and_format_by_term(name: str) ‑> List[str]`
:   Same as `name_split_and_format` but ensures that each item in the list is an
    entire term.
    
    A term is defined as a word, initialism, number or sequence of
    non-delimiting characters (see `name_split_and_format` for which characters
    are considered delimiters).
    
    For example, whereas `name_split_and_format` would split
    'Send(FSW::Says_Hello55)'
    into:
    ['Send(', 'F', 'S', 'W', 'Says', 'Hello55)']
    `name_split_and_format_by_term` would split it into:
    ['Send', '(', FSW', 'Says', 'Hello', '55', ')'].
    
    The main thing this process adds is splitting off non-alphabetic symbols,
    identifying boundaries between initialisms and words.
    
    # Examples:
    # Basic:
    >>> name_split_and_format_by_term('SNRLevel0')
    ['SNR', 'Level', '0']
    
    # Slightly challenging:
    >>> name_split_and_format_by_term('FSW::Says_Hello55')
    ['FSW', 'Says', 'Hello', '55']
    
    # Slightly challenging w/adjacent digits:
    >>> name_split_and_format_by_term('FSW::Says_Hello56_5')
    ['FSW', 'Says', 'Hello', '56', '5']
    
    # Moderately challenging:
    >>> name_split_and_format_by_term('send(FSW::Says_Hello55)')
    ['Send', '(', 'FSW', 'Says', 'Hello', '55', ')']
    
    # Involves all challenge cases:
    >>> name_split_and_format_by_term('A1::ABC2::FSW((say_phrase)): AHelloWorld--1Time_in55seconds_ToGSW')
    ['A', '1', 'ABC', '2', 'FSW', '((', 'Say', 'Phrase', '))', 'A', 'Hello', 'World', '1', 'Time', 'In', '55', 'Seconds', 'To', 'GSW']

    
`partial_format_string(txt: str, **kwargs) ‑> str`
:   Partially format the given string by key and leave unformatted values still
    in the string for later formatting.
    
    # Examples:
    >>> partial_format_string('({a}, {b}, {c})', a=1, c=3)
    '(1, {b}, 3)'
    >>> partial_format_string('({a}, {b}, {c})', a='1', c="3")
    '(1, {b}, 3)'
    >>> partial_format_string('({a}, {b}, {c})', b='bee')
    '({a}, bee, {c})'
    >>> partial_format_string('({a}, {b}, {c})', b='bee', d='dee')
    '({a}, bee, {c})'
    >>> partial_format_string(partial_format_string('({a}, {b}, {c})', a=1, c=3), b='bee', d='dee')
    '(1, bee, 3)'

    
`print_bytearray_hex(ba) ‑> None`
:   

    
`type_guard_argument(arg_type: Union[Type, Tuple[Type, ...]], arg_name: str, arg_val: Any, attempt_cast: bool = True, calling_function_name: Optional[str] = None) ‑> Any`
:   A guard to add to the beginning of a function to make sure one of its
    arguments has one of the allowed types.
    
    Args:
        var_type (Union[Type, Tuple[Type, ...]]): Allowable type(s) for argument
        var_name (str): Name of argument (from parameters list ideally)
        var_val (Any): Value of argument given
        attempt_cast (bool, optional): Whether to attempt to cast. Defaults to True.
        calling_function_name (str, optional): Name of function which had the arg 
            being checked. Defaults to None, in which case the name of the calling 
            function is grabbed from the stack.
    
    Raises:
        ValueError: Error if value is not one of the specified types.
    
    Returns:
        Any: value given, cast to the correct type if needed

Classes
-------

`PartialFormatStringDict(*args, **kwargs)`
:   Dict extension used for string formatting which returns a format string
    template for unknown keys.
    
    Used in `partial_format`.

    ### Ancestors (in MRO)

    * builtins.dict