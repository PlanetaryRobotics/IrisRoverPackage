"""
Helper functions for building AIO components per the pattern here:
https://dash.plotly.com/all-in-one-components

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/08/2023
"""

from typing import Any, Callable, Generic, TypedDict, TypeVar, Dict, Type, List

AIO_T = TypeVar('AIO_T', bound=object)


class AioSubId(TypedDict):
    component: str  # e.g. 'MarkdownWithColorAIO'
    subcomponent: str  # e.g. 'dropdown'
    aio_id: Any  # e.g. aio_id


def id_generator(
    component_name: str,
    sub_component_name: str
) -> Callable[[Any], AioSubId]:
    """Template for an ID lookup function."""
    def lookup_fn(aio_id: Any) -> AioSubId:
        return AioSubId(
            component=component_name,
            subcomponent=sub_component_name,
            aio_id=aio_id
        )
    return lookup_fn


def props_guard(props: Dict | None) -> Dict:
    """Simple guard for an optional properties dictionary to make a new empty
    dict is no dict is given."""
    return dict() if props is None else props
