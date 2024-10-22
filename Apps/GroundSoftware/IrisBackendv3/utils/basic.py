# -*- coding: utf-8 -*-
"""
A Collection of Basic Utility Functions.

@author: Connor W. Colombo (CMU)
@last-updated: 09/17/2022
"""

import re
import string
from typing import Optional, Type, Dict, Tuple, List, Any, Union, TypeVar, cast
import inspect

_T = TypeVar('_T')


def type_guard_argument(
    arg_type: Union[Type[_T], Tuple[Type[_T], ...]],
    arg_name: str,
    arg_val: Any,
    attempt_cast: bool = True,
    calling_function_name: Optional[str] = None
) -> _T:
    """A guard to add to the beginning of a function to make sure one of its
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
    """

    if isinstance(arg_type, list):
        # If valid types is a list, convert to tuple for `isinstance`:
        arg_type = tuple(arg_type)

    if isinstance(arg_type, type):
        # Just one type was given, wrap it in a 1-tuple so it's iterable:
        arg_type = (arg_type, )

    if not isinstance(arg_val, arg_type):
        # Given value does not match any of the allowed types:
        cast_success = False
        if attempt_cast:
            # Attempt to cast `arg_val` to any of the allowed types:
            i: int = 0
            while not cast_success and i < len(arg_type):
                try:
                    arg_val = arg_type[i](arg_val)  # type: ignore
                    cast_success = True
                except ValueError:
                    # try again
                    i += 1

        if not attempt_cast or attempt_cast and not cast_success:
            # Get the name of the function that called this one (for message):
            if calling_function_name is None:
                calling_function_name = inspect.stack()[1][3]
            # Raise an error saying the caller got an unexpected argument:
            raise ValueError(
                f"`{calling_function_name}` expects `{arg_name}` to be "
                f"`{arg_type}`, "
                f"instead it had type `{type(arg_val)}` and value `{arg_val}`."
            )

    return cast(_T, arg_val)


def bytearray_to_spaced_hex(ba, spacer: str = ' ') -> str:
    return spacer.join(["{:02x}".format(x) for x in ba])


def print_bytearray_hex(ba) -> None:
    print(bytearray_to_spaced_hex(ba))


def hexstr_to_bytes(hexstr: str, delimiter: str = ' '):
    """
    Converts the given hex string, delimited with the given delimiter, to a
    bytes object.

    # Examples:
    >>> hexstr_to_bytes('52 41 44 3A 45 43 48 4F 7C 68 48 45 4C 4C 4F', ' ')
    b'RAD:ECHO|hHELLO'
    """
    return bytes.fromhex(hexstr.replace(delimiter, ''))


def byte_to_binstr(B):
    # Convert to bin string:
    binstr = bin(B)[2:]
    # Pad with appropriate number of zeros:
    binstr = '0' * (8-len(binstr)) + binstr
    return binstr


def bytes_to_binstr(Bs):
    return '0b '+' '.join(byte_to_binstr(b) for b in Bs)


def flip_bits_in_byte(one_byte: Union[int, bytes]) -> Union[int, bytes]:
    """
    Reverses the order of all bits in the given byte (as an int or a 1 byte long bytes object).
    """
    if isinstance(one_byte, bytes):
        byte_as_int: int = one_byte[0]
    else:
        byte_as_int = one_byte & 0xFF
    lookup = [0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6,
              0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf]

    flipped = (lookup[byte_as_int & 0x0F] << 4) | lookup[byte_as_int >> 4]

    if isinstance(one_byte, bytes):
        return flipped.to_bytes(1, 'big')
    else:
        return flipped


def flip_all_bits_in_bytes(bs: bytes) -> bytes:
    """
    Reverses the order of all bits in the bytes object

    # Examples:
    >>> bytes_to_binstr(flip_all_bits_in_bytes(0b111111101110110111011110101011011011111011101111.to_bytes(6,'big')))
    '0b 11110111 01111101 10110101 01111011 10110111 01111111'
    """
    lookup = [0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6,
              0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf]
    # Flip the endianness:
    rev = bytes(reversed(bs))
    # Then make a mutable copy of the array which will have each of its bytes progressively flipped:
    flipped = bytearray(rev)
    for i in range(0, len(rev)):
        flipped[i] = (lookup[rev[i] & 0x0F] << 4) | lookup[rev[i] >> 4]

    return bytes(flipped)


def dict_field_type_check(
    data: Dict,
    field: str,
    desired_type: Union[type, List[type]],
    name: str | None = None
) -> None:
    """
    Check that the given field of a dict has the desired type. If not, raise a TypeError.

    Optionally, a `name` can be provided which matches the variable name of the
    given `data` dict in the calling environment and will be displayed in the
    event of a type error.

    `desired_type` can also be a list of types, only one of which need to match.
    """
    if name is None:
        name = 'data'

    if isinstance(desired_type, list):
        if not any([isinstance(data[field], dt) for dt in desired_type]):
            raise TypeError(
                f"`{name}['{field}']` should be any of {[dt.__name__ for dt in desired_type]}. "
                f"Got {data[field]} which is {type(data[field]).__name__}."
            )
    else:
        if not isinstance(data[field], desired_type):
            raise TypeError(
                f"`{name}['{field}']` should be {desired_type.__name__}. "
                f"Got {data[field]}. which is {type(data[field]).__name__}."
            )


def dict_type_check(
    data: Dict,
    type_dict: Dict[str, Union[type, List[type]]],
    name: str | None = None
) -> None:
    """
    Check that all fields in the given dictionary have the desired types. If not, raise a TypeError.

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
    """
    for k in type_dict.keys():
        dict_field_type_check(data, k, type_dict[k], name)


def full_dict_spec_check(
    data: Dict,
    spec_dict: Dict[str, Union[type, List[type]]],
    name: str | None = None
) -> None:
    """
    Check that the given `data` dictionary has, at least, all the fields
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
    """
    if name is None:
        name = 'data'

    if not isinstance(data, dict):
        raise TypeError(f"`{name}` should be dict. Got {data}.")

    given_keys = set(data.keys())
    targ_keys = set(spec_dict.keys())
    if not given_keys >= targ_keys:
        raise ValueError(
            f"`{name}` should have keys {targ_keys}. Got {given_keys}."
        )

    dict_type_check(data, spec_dict, name)


class PartialFormatStringDict(dict):
    """
    Dict extension used for string formatting which returns a format string
    template for unknown keys.

    Used in `partial_format`.
    """

    def __missing__(self, key: str) -> str:
        return '{' + key + '}'


def partial_format_string(txt: str, **kwargs) -> str:
    """
    Partially format the given string by key and leave unformatted values still
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
    """

    return string.Formatter().vformat(txt, (), PartialFormatStringDict(**kwargs))


def name_split_and_format(name: str) -> List[str]:
    """
    Split a name string by all typical delimiters and return its pieces with consistent formatting.

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
    >>> name_split_and_format('hello   \t  world')
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
    """
    # Split on Special Characters:
    pieces: List[Any] = re.split(r'\s+|_+|-+|:+', name)
    # Split on Classes (Symbols, Numbers, and Capitals) and build a list of lists:
    for i, p in enumerate(pieces):
        pieces[i] = re.findall(
            # Any
            # Symbols:
            r"[^A-z0-9]+"  # not alphanumeric
            # OR words:
            # Sequences starting with at most 1 capital then lower case then optionally numbers:
            r"|[A-Z]?[a-z]+[0-9]*"
            # then isolate all remaining capital letters:
            r"|[A-Z]"
            r"|[^A-Z]+", p)
    # Flatten compound list to get all pieces:
    pieces = [p for l in pieces for p in l]
    # Capitalize only first letter of each piece (all other letters are
    # lowercase implicitly):
    for i, p in enumerate(pieces):
        first, *rest = p
        pieces[i] = "".join([first.upper(), *rest])

    return pieces


def name_split_and_format_by_term(name: str) -> List[str]:
    """
    Same as `name_split_and_format` but ensures that each item in the list is an
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

    """
    # Format into basic PascalCase to start with standard boundaries:

    # Build the name with no joiner between pieces.
    # *but* Make sure to preserve boundaries between adjacent digits.
    # Since digits can't be capitalized and thus the boundaries will be erased.
    pieces = name_split_and_format(name)
    pascal = ""
    for i in range(len(pieces)-1):
        pascal += pieces[i]
        if pieces[i][-1].isdigit() and pieces[i+1][0].isdigit():
            # If this piece ends w/a digit and next one starts with one,
            # add a boundary to preserve the boundary that was once there (so it
            # doesn't just look like one big number):
            pascal += '_'  # this will be filtered out as a separator later
    pascal += pieces[-1]

    # Split and Return:
    return re.findall(
        # Any:
        # Sequences of symbols:
        r"[^A-z0-9]+"  # not alphanumeric
        # OR initialisms:exi
        r"|[A-Z]+"  # A sequence of all capital letters
        r"(?=[^a-z]{1}|$)"  # then non-capital (not captured) or end of string:
        # OR word (already in PascalCase):
        r"|[A-Z][a-z]+"  # 1 capital then >=1 non-capitals
        # OR number:
        r"|[0-9]+"  # sequence of digits
        , pascal)


if __name__ == "__main__":
    # Run cheap doctests on built-in examples for any util functions:
    import doctest
    print(f"\n{doctest.testmod()}\n")
    print("Doctest Done!")
