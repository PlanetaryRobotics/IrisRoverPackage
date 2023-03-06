"""
Defines the container structure for messages that pass through `Topic`s
between IPC Apps.

NOTE: This code used to live inside `wrapper.py`

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods)
from __future__ import annotations
from abc import ABC, abstractmethod
from typing import Any, Generic, Optional, TypeVar, Protocol, List, TypeGuard, Dict, Type
import attr
import typeguard

from IrisBackendv3.ipc.exceptions import DecodedInterProcessMessageContentTypeException
from IrisBackendv3.ipc.restricted_pickler import IRIS_RESTRICTED_PICKLER_VERSION
from IrisBackendv3.ipc.serializer import IrisGenericSerializer, IRIS_SERIALIZER_VERSION
from IrisBackendv3.ipc.logging import logger

# Create special serializer for messages:
_MessageSerializer = IrisGenericSerializer()


# Inter-Process Message Content:
IPMC = TypeVar('IPMC', bound='MessageContentInterface')

# InterProcessMessage or Subclass:
_IPMT = TypeVar('_IPMT', bound='InterProcessMessage')


class MessageContentInterface(ABC):
    """Basic interface specifying that all `content` classes must be able to be
    decomposed to a dict of objects that can be serialized using
    `IrisGenericSerializer` (i.e. they are on the approved modules list) using
    a method called `to_dict` and rebuilt using a method called
    `from_dict`."""

    def encode(self) -> Dict:
        """Basically just `to_dict` but adds a field that includes the class
        name to make sure the class rebuilt on the other end is the correct
        class, not just a class with the same fields.
        Also adds a check for the restricted pickler and serializer versions,
        to act as a compatibility check."""
        return {
            **self.to_dict(),
            '__class__': self.__class__.__name__,
            '_vIRP': IRIS_RESTRICTED_PICKLER_VERSION,
            '_vIS': IRIS_SERIALIZER_VERSION
        }

    @classmethod
    def decode(cls: Type[IPMC], d: Dict) -> IPMC:
        """Basically just `from_dict` but checks a field that includes the
        class name to make sure the class rebuilt on the other end is the
        correct class, not just a class with the same fields.
        Also checks the restricted pickler and serializer versions,
        to act as a compatibility check."""
        # Check for Iris Restricted Pickler and Serializer versions for
        # compatibility:
        k_vIRP, k_vIS = '_vIRP', '_vIS'
        if k_vIRP not in d or k_vIS not in d:
            raise ValueError(
                f"In `MessageContentInterface.decode`, message content dict "
                f"is missing either or both of the mandatory version fields: "
                f"`{k_vIRP}`, `{k_vIS}`. "
                f"Dict: `{d}`."
            )
        if d[k_vIRP] != IRIS_RESTRICTED_PICKLER_VERSION:
            # To prevent thing from breaking, just log a very annoying error.
            # Don't raise an exception outright.
            logger.error(
                f"Iris Restricted Pickler Version in received message "
                f"({d[k_vIRP]}) doesn't match the version in this instance of "
                f"the backend ({IRIS_RESTRICTED_PICKLER_VERSION=}). "
                f"It's possible an incompatible version of the IrisBackend "
                f"was used to encode this message. Sync versions ASAP."
            )
        if d[k_vIS] != IRIS_SERIALIZER_VERSION:
            # To prevent thing from breaking, just log a very annoying error.
            # Don't raise an exception outright.
            logger.error(
                f"Iris Serializer Version in received message "
                f"({d[k_vIS]}) doesn't match the version in this instance of "
                f"the backend ({IRIS_SERIALIZER_VERSION=}). "
                f"It's possible an incompatible version of the IrisBackend "
                f"was used to encode this message. Sync versions ASAP."
            )

        k_class = '__class__'
        if k_class not in d:
            raise ValueError(
                f"In `MessageContentInterface.decode`, message content dict "
                f"doesn't include the mandatory {k_class} field. "
                f"Dict: `{d}`."
            )
        if d[k_class] != cls.__name__:
            raise ValueError(
                f"In `MessageContentInterface.decode`, "
                f"message content class indicated by `{d[k_class]=}` "
                f"doesn't match the message content class trying to be "
                f"rebuilt `{cls.__name__=}`."
            )
        # Remove meta-fields and rebuild:
        del d[k_vIRP]
        del d[k_vIS]
        del d[k_class]
        return cls.from_dict(d)

    @abstractmethod
    def to_dict(self) -> Dict: raise NotImplementedError()

    @classmethod
    @abstractmethod
    def from_dict(cls: Type[IPMC], d: Dict) -> IPMC:
        raise NotImplementedError()


class MessageContentAttrMixin(MessageContentInterface):
    """Mixin that adds `to_tuple` functionality to any `MessageContent` classes
    made using `attr`."""

    def to_dict(self: attr.AttrsInstance) -> Dict:
        return attr.asdict(self)

    @classmethod
    def from_dict(cls: Type[IPMC], d: Dict) -> IPMC: return cls(**d)


class InterProcessMessage(Generic[IPMC], ABC):
    """
    Interface for any message data which supports being sent between processes.
    """

    __slots__: List[str] = ['content']

    content: IPMC

    @staticmethod
    @abstractmethod
    def CONTENT_TYPE() -> Type[IPMC]:
        """
        The type/class that `content` is checked against **at runtime**.
        This should be the same as `IPMC`.
        This has to be handled as a separate variable b/c it's difficult to
        reliably check types against a type-hint (esp. a `TypeVar`) at runtime,
        particularly in a way that doesn't depend on private python
        implementations and is supported across many python versions.
        NOTE: `mypy` should be able to automatically check to make sure a
        compatible type is returned here. Again, this should be **the same** as
        what is supplied for `IPMC`. `IpmSubclassFactory` enforces this.
        NOTE: This effectively needs to be an abstract static class variable,
        but since that's not supported by python, an abstract staticmethod
        which returns a fixed result is functionally equivalent.
        """
        raise NotImplementedError("No `CONTENT_TYPE`.")

    def __init__(self, content: IPMC) -> None:
        typeguard.check_type('content', content, self.CONTENT_TYPE())
        self.content = content

    def __repr__(self) -> str:
        return f"IPM(content = {self.content})"

    def __eq__(self, other: Any) -> bool:
        return (
            isinstance(other, InterProcessMessage)
            and self.CONTENT_TYPE() == other.CONTENT_TYPE()
            and self.content == other.content
        )

    def to_ipc_bytes(self) -> bytes:
        """
        Pack this object into bytes to be sent over the IPC network
        (in a safe way, unlike pickle).
        Default behavior just uses `ipc.serializer` to de/serialize
        `content` but this can be overridden in a subclass.
        """
        return _MessageSerializer.serialize(self.content.encode())

    @classmethod
    def _is_content_type(cls, content: IPMC) -> TypeGuard[IPMC]:
        """Narrowing `TypeGuard` that makes sure the given value is `IPMC`
        **at runtime**."""
        return isinstance(content, cls.CONTENT_TYPE())

    @classmethod
    def from_ipc_bytes(cls: Type[_IPMT], data: bytes) -> _IPMT:
        """
        Unpack bytes sent over IPC to reconstruct the sent object.
        Default behavior just uses `ipc.serializer` to de/serialize
        `content` but this can be overridden in a subclass.
        Since the data unpacked could hypothetically have any type allowed by
        the restricted unpickler, this will validate that the unpacked data has
        `cls.CONTENT_TYPE`.
        """
        # Deserialize:
        content_dict: Any = _MessageSerializer.deserialize(data)
        # Build content:
        content = cls.CONTENT_TYPE().decode(content_dict)
        # Ensure the correct type:
        if cls._is_content_type(content):
            return cls(content=content)
        else:
            raise DecodedInterProcessMessageContentTypeException(
                content,
                cls.CONTENT_TYPE()
            )


def IpmSubclassFactory(
    ipmc_t: Type[IPMC]
) -> Type[InterProcessMessage[IPMC]]:
    """Factory that handles the boiler plate of creating a basic subclass of
    `InterProcessMessage` (basically removes the repetition).

    Subclasses that require special functionality can still be made normally,
    just make sure to include any of the necessary boilerplate included here.
    """
    class SubIpm(InterProcessMessage[IPMC]):
        @staticmethod
        def CONTENT_TYPE() -> Type[IPMC]:
            return ipmc_t

    return SubIpm


class IPMHandler(Protocol):
    """ Structural typing `Protocol` defining a handler that decodes raw
    IPC message bytes, performs some operation, and optionally returns an
    `InterProcessMessage` if needed by the process type (i.e. server/client).

    NOTE:
    Can't do something as simple as `Callable[[bytes], InterProcessMessage])`
    b/c of this still open (as of 2/26/22) issue:
    https://github.com/python/mypy/issues/5485
    which prevents `subtopic.handler(ipc_raw.msg)` or even
    `subtopic.handler.__call__(ipc_raw.msg)` from working.
    """

    def __call__(self, __raw_msg: bytes) -> Optional[InterProcessMessage]: ...


def IsIPMHandler(func: IPMHandler) -> IPMHandler:
    """ Decorator to use before a function to have `mypy` make sure it complies
    with the `IPMHandler` callback protocol.
    Example:
    ```py
    @IsIPMHandler
    def good_ipmh(msg_bytes: bytes) -> InterProcessMessage:
        pass
    ```
    creates no errors.

    ```py
    @IsIPMHandler
    def bad_ipmh(x: int, y: Dict) -> float:
        pass
    ```
    gives a mypy error b/c it's argument and return type signatures are wrong.

    but
    """
    return func
