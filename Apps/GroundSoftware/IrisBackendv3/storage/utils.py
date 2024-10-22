"""
Common utilities for handling storage data.
"""
from typing import Iterable, List

import re


def _text_to_int_if_int(text: str) -> int | str:
    return int(text) if text.isdigit() else text


def _alphanum_key(key: str) -> List[int | str]:
    return [
        _text_to_int_if_int(c)
        for c in re.split('([0-9]+)', key)
    ]


def sorted_natural_ints(l: Iterable[str]) -> List[str]:
    """
    Sort a list alphanumerically, with natural sorting of embedded integers
    Derived from: Mark Byers, Stack Overflow
    (https://stackoverflow.com/a/2669120) [CC BY-SA 2.5]
    """
    return sorted(l, key=_alphanum_key)
