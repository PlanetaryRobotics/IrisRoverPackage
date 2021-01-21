#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This module contains tools which serve as the standard bearer for how 
GSW-specific metadata is embedded in the FPrime XML.

@author: Connor W. Colombo (CMU)
@last-updated: 01/18/2021
"""

from lxml import etree
import re

from .exceptions import StandardsFormattingException


def extract_from_xml(node: etree.Element) -> str:
    """
    Extract the GSW-specific metadata from the given XML node (eg. `<command>` or `<arg>`).
    """
    comment_src = node.xpath("./comment")
    if len(comment_src) == 0:
        raise StandardsFormattingException(
            node.base,
            f"At sourceline {node.sourceline}, node doesn't have a comment."
        )
    elif len(comment_src) > 1:
        raise StandardsFormattingException(
            node.base,
            (
                f"At sourceline {node.sourceline}, node somehow has multiple "
                "comments which are direct descendants. Consider merging. "
                "Only the first will be used."
            )
        )
    else:
        comment = comment_src[0].text
        return extract_from_str(comment)


def extract_from_str(comment: str) -> str:
    """Extract the GSW-specific metadata stored in the given comment string."""
    search = re.search(r"{JSONSTART{([\s\S]*)}JSONEND}", comment)
    if search is None:
        return r"{}"  # No match -> empty JSON string

    return search.group(1)


def build(metadata: str) -> str:
    """Build a GSW-specific metadata comment string out of the given encoded metadata string."""
    return r"{JSONSTART{" + metadata + r"}JSONEND}"


def insert_into_xml(node: etree.Element, metadata: str) -> None:
    """
    Format and insert the given metadata into the comment of the given XML node as CDATA.

    If the element doesn't have a comment yet, one will be added.
    """
    # TODO
    raise NotImplementedError()
