# -*- coding: utf-8 -*-
"""
A Collection of Basic Utility Functions.

@author: Connor W. Colombo (CMU)
@last-updated: 01/20/2021
"""

import string
from typing import Dict, List, Any


def print_bytearray_hex(ba):
    print(" ".join(["{:02x}".format(x) for x in ba]))


def dict_field_type_check(data: Dict, field: str, desired_type: type, name: str = None) -> None:
    """
    Check that the given field of a dict has the desired type. If not, raise a TypeError.

    Optionally, a `name` can be provided which matches the variable name of the 
    given `data` dict in the calling environment and will be displayed in the 
    event of a type error.
    """
    if name is None:
        name = 'data'
    if not isinstance(data[field], desired_type):
        raise TypeError(
            f"`{name}['{field}']` should be {desired_type.__name__}. Got {data[field]}."
        )


def dict_type_check(data: Dict, type_dict: Dict[str, type], name: str = None) -> None:
    """
    Check that all fields in the given dictionary have the desired types. If not, raise a TypeError.

    Desired types are given in a type dictionary where each key matches a key in 
    the data dictionary being evaluated, and each value is the desired type.

    Optionally, a `name` can be provided which matches the variable name of the 
    given `data` dict in the calling environment and will be displayed in the 
    event of a type error.

    Example:
    To make sure the `name` field in the given `data` dictionary is a `str` and 
    the `ID` field is an `int`, the following `type_dict` must be given:
    type_dict = {'name': str, 'ID': int}
    """
    for k in type_dict.keys():
        dict_field_type_check(data, k, type_dict[k], name)


def full_dict_spec_check(data: Dict, spec_dict: Dict[str, type], name: str = None) -> None:
    """
    Check that the given `data` dictionary has, at least, all the fields 
    specified in keys of `spec_dict` and that all of those fields have the 
    types specified by the values of `spec_dict`. If not a KeyError or 
    TypeError is raised as appropriate.

    Optionally, a `name` can be provided which matches the variable name of the 
    given `data` dict in the calling environment and will be displayed in the 
    event of a type error.

    Example:
    To make sure the given `data` dict contains the keys `name` and `field` and 
    the that name` field in the given `data` dictionary is a `str` and the `ID` 
    field is an `int`, the following `spec_dict` must be given:
    spec_dict = {'name': str, 'ID': int}
    """
    if name is None:
        name = 'data'

    if not isinstance(data, dict):
        raise TypeError(f"`{name}` should be dict. Got {data}.")\

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


def partial_format(txt: str, **kwargs) -> str:
    """
    Partially format the given string by key and leave unformatted values still 
    in the string for later formatting.

    # Examples:
    >>> partial_format('({a}, {b}, {c})', a=1, c=3)
    '(1, {b}, 3)'
    >>> partial_format('({a}, {b}, {c})', a='1', c="3")
    '(1, {b}, 3)'
    >>> partial_format('({a}, {b}, {c})', b='bee')
    '({a}, bee, {c})'
    >>> partial_format('({a}, {b}, {c})', b='bee', d='dee')
    '({a}, bee, {c})'
    >>> partial_format(partial_format('({a}, {b}, {c})', a=1, c=3), b='bee', d='dee')
    '(1, bee, 3)'
    """

    return string.Formatter().vformat(txt, (), PartialFormatStringDict(**kwargs))


def name_split_and_format(name: str) -> List[str]:
    """
    Split a name string by all typical delimiters and return its pieces with consistent formatting.

    # Details:
    Given a name string where components of the name could be delimited by
    certain special characters (_, -, : , or whitespace) or capitalization,
    split on all possible boundaries and the format each piece by ensuring that
    only the first letter is capitalized.

    Returns a list of all the pieces.

    # Examples:
    ## Trivial:
    >>> name_split_and_format('hello')
    ['Hello']
    >>> name_split_and_format('Hello')
    ['Hello']
    >>> name_split_and_format('101Dalmatians')
    ['101', 'Dalmatians']

    ## Split on Any number of Special Characters:
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

    ## Split on Capitalization:
    >>> name_split_and_format('helloWorld')
    ['Hello', 'World']

    ## With other characters:
    >>> name_split_and_format('hello55World99')
    ['Hello55', 'World99']

    ## Combination:
    >>> name_split_and_format('say::hello55World99')
    ['Say', 'Hello55', 'World99']
    """
    import re
    # Split on Special Characters:
    pieces: List[Any] = re.split('\s+|_+|-+|:+', name)
    # Split on Capital Letters and build a list of lists:
    for i, p in enumerate(pieces):
        pieces[i] = re.findall('[A-Z][^A-Z]*|[^A-Z]+', p)
    # Flatten compound list to get all pieces:
    pieces = [p for l in pieces for p in l]
    # Capitalize only first letter of each piece (all other letters are
    # lowercase implicitly):
    for i, p in enumerate(pieces):
        first, *rest = p
        pieces[i] = "".join([first.upper(), *rest])

    return pieces


if __name__ == "__main__":
    # Run cheap doctests on built-in examples for any util functions:
    import doctest
    doctest.testmod()
    print("Doctest Done!")
