"""
Class whose instances define an IPC topic, including what messages are passed
on the topic.

@author: Connor W. Colombo (CMU)
@last-updated: 03/03/2023
"""
from dataclasses import dataclass

from typeguard import check_type
from typing import _BaseGenericAlias  # type: ignore
from typing import _SpecialForm, List, Any, Type, Tuple, Callable
from typing_extensions import TypeAlias

from IrisBackendv3.ipc.inter_process_message import InterProcessMessage

AnyType: TypeAlias = Type | _BaseGenericAlias | Tuple | Callable | _SpecialForm


@dataclass
class TopicDefinition:
    # Definition (class) of Message that gets sent on this topic:
    message_def: Type[InterProcessMessage]

    @property
    def topic_type(self) -> AnyType:
        """Instance attributes for type-checker:
        Type of data written to the topic (can be any valid mypy type):"""
        return self.message_def.CONTENT_TYPE()

    def allows(self, data: Any) -> bool:
        """
        Checks whether the given data is allowed on the given topic based on
        its type.
        """
        try:
            check_type('data', data, self.topic_type)
            return True
        except TypeError:
            return False
