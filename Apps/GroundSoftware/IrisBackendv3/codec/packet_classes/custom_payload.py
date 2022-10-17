"""
Abstract implementation of `CustomPayloadPacket` which is used as superclass
for Packets which extract a number of Payloads (`CustomPayload`) from a single
large payload, often, in part, as a bitfield. This is used almost exclusively 
for communication with the Watchdog which doesn't use the Iris Common Packet
format for its telemetry (to minimize bandwidth use).

@author: Connor W. Colombo (CMU)
@last-updated: 05/09/2022
"""
from __future__ import annotations  # Activate postponed annotations (for using classes as return type in their own methods)

from .packet import Packet, CT

from typing import List, Optional, ClassVar, TypeVar, cast, Generic, Type, Dict

import time

from ..magic import Magic
from ..payload import TelemetryPayload
from ..payload_collection import EnhancedPayloadCollection

from ..settings import ENDIANNESS_CODE, settings

from IrisBackendv3.data_standards.module import Module


CPCT = TypeVar('CPCT')  # CustomPayload Class Type


class CustomPayloadPacket(Packet[CT], Generic[CT, CPCT]):
    """
    Superclass for a special **non**-Iris Common Packet packet
    (e.g. generated by the Watchdog) which contains items which would normally
    be in separate telemetry channels but instead are contained in one
    continuous `CustomPayload`. This class maps the extracts the values in the
    `CustomPayload` and stores them in traditional `TelemetryPayload`s linked to
    a custom `TelemetryChannel`.
    All of these telemetry channels must be found
    in a `DataStandards` module, typically a `prebuilt` one.

    **The subclass must specify a `PREBUILT_MODULE_NAME` which links the subclass
    to the `DataStandards` module it represents.**

    Members of the custom payload must have same names as the corresponding
    telemetry channels in the prebuilt `DataStandards` module.
    Note: an error will get thrown by `CustomPayloadPacket.__init__`
    when building `payloads` from the `custom_payload` if all the channels
    in the `PREBUILT_MODULE_NAME prebuilt module don't have a corresponding
    attr in this `CustomPayload`.

    *NOTE:* For this to work effectively:
    1. All fields, including computed properties, in the custom payload must
        match the names of the corresponding telemetry channels from the
        prebuilt module.
    2. Although the custom payload class can have fields that don't appear as
        telemetry channels in the linked datastandards module, all telemetry
        channels in the module must have matching fields in the custom payload
        class.
    3. All args to `__init__` of the custom payload must have corresponding
        channels in the linked datastandards module.
    4. The order of the args in `__init__` of the custom payload must
        match the order of the bytes/bits in the packet.
    """
    # Name of the Module in the linked datastandards that the implementing
    # `CustomPayloadPacket` encodes:
    PREBUILT_MODULE_NAME: ClassVar[Optional[str]] = None
    # Class that contains the values of all payloads in this custom payload
    # class (used as an intermediary between the packet bytes which contain all
    # those fields and the `payloads` `EnhancedPayloadCollection`):
    CUSTOM_PAYLOAD_CLASS: Optional[Type[CPCT]] = None

    __slots__: List[str] = [
        '_custom_payload'
    ]

    @classmethod
    def get_ds_module(cls) -> Module:
        """
        Returns the corresponding prebuilt `DataStandards` `Module` used for
        mapping this packet's data to telemetry streams.
        """
        if cls.PREBUILT_MODULE_NAME is None:
            raise NotImplementedError(
                f"{cls} is a subclass of `CustomPayloadPacket` "
                "which must provide a `PREBUILT_MODULE_NAME` class variable "
                "but doesn't."
            )

        try:
            return settings['STANDARDS'].modules[cls.PREBUILT_MODULE_NAME]
        except KeyError:
            raise TypeError(
                f"Attempted to parse a `{cls.__name__}` which "
                f"requires the `{cls.PREBUILT_MODULE_NAME}` special `prebuilt` "
                "module to be loaded into the standards but it can't be found."
            )

    @property
    def custom_payload(self) -> CPCT:
        # public getter, private setter
        return self._custom_payload

    @property
    def payloads(self) -> EnhancedPayloadCollection:
        return self._payloads

    @payloads.setter
    def payloads(self, payloads: EnhancedPayloadCollection) -> None:
        """Override default `Packet` setter so the locally cached 
        `_custom_payload` can be updated if `payloads` (which drives it) is
        changed.

        NOTE: The following was checked to make sure that even if a superclass
        calls `payloads=` (e.g. in `__init__`), it will be dispatched to this
        subclass overridden version:

        ```python
            class A():
                __slots__ = ['_thing']
                _thing: str          
                @property
                def thing(self) -> str:
                    print(f"Super class thing getter {self._thing}")
                    return self._thing
                @thing.setter
                def thing(self, thing: str) -> None:
                    print(f"Super class thing setter {thing}")
                    self._thing = thing
                def __init__(self, t: str) -> None:
                    self.thing = t

            class B(A):
                __slots__ = [] # propagate `__slots__` from parent
                @property
                def thing(self) -> str:
                    print(f"Sub class thing getter {self._thing}")
                @thing.setter
                def thing(self, thing: str) -> None:
                    print(f"Sub class thing setter {thing}")
                    self._thing = thing
                def __init__(self, t: str) -> None:
                    super().__init__(t=t)

            b = B('bb')  # -> prints "Sub class thing setter bb"
            b.thing  # -> prints "Sub class thing getter bb"
        ```
        """
        if self._payloads != payloads:
            # only recompute if a change is actually occuring (necessary to
            # avoid needing to recompute in the constructor since it sets
            # `_custom_payload` then `_payloads` and then has to dispatch to
            # `super`'s `__init__` which will call this setter again.
            # store this first in case issues arise with unpacking and an error is raised
            self._payloads = payloads
            self._custom_payload = self.pack_payloads_into_custom_payload(
                payloads)
        else:
            self._payloads = payloads

    def pack_payloads_into_custom_payload(
        self,
        payloads: EnhancedPayloadCollection,
    ) -> CPCT:
        """Reconstructs the `custom_payload` (instance of
        `cls.CUSTOM_PAYLOAD_CLASS`) which was used to construct all the
        payloads.
        `custom_payload` is used as an intermediary between the packet bytes
        which contain all the fields used to create the `payloads`
        `PayloadCollection`.
        After construction, this field is normally redundant since it
        necessarily contains the same data as in `payloads`.
        This getter is required to maintain backward compatibility with legacy
        code written when `custom_payload` was a member of all
        `CustomPayloadPacket` classes (and thus expected to exist).
        The `custom_payload` field is also convenient since it allows for a
        compact representation of all data in al fields and
        `cls.CUSTOM_PAYLOAD_CLASS` classes often contain a concise way of
        handling and displaying all the data they contain.

        NOTE: This does not automatically replace `_custom_payload`, though you could
        do that by `self._custom_payload=self.unpack_custom_payload_to_payloads(...)`
        if desired.
        """

        # Go through every telemetry channel in the linked `DataStandards`
        # `Module` and lookup the value in the Custom Payload.
        module = self.get_ds_module()

        custom_payload_args = dict()

        for payload in payloads[TelemetryPayload]:
            payload = cast(TelemetryPayload, payload)
            try:
                channel = module.telemetry[payload.channel_id]
            except KeyError as e:
                raise ValueError(
                    f"`CustomPayloadPacket` class {self.__class__} was "
                    f"decoding its internal `payloads` when it encountered a "
                    f"channel id `{payload.channel_id}` which was not contained "
                    f"in the registered module `{module}`. Have the "
                    "datastandards been changed since this data was encoded? "
                    "If so, consider using a cached datastandards (`*.dsc`) "
                    "file from the time when this data was first encoded. "
                    "Note: this data is still contained in an uncorrupted and "
                    "readable form, just certain debugging functions (likely "
                    "including printing) will be non-functional until correct "
                    "datastandards are used. "
                    f"KeyError given: {e}."
                )
            custom_payload_args[channel.name] = payload.data

        if self.CUSTOM_PAYLOAD_CLASS is None:
            raise NotImplementedError(
                f"`CustomPayloadPacket` class {self.__class__} appears to not"
                "be fully implemented since a `CUSTOM_PAYLOAD_CLASS` class "
                "is required so the class can know how to unpack this data but "
                "no `CUSTOM_PAYLOAD_CLASS` ClassVar was given."
            )
        else:
            c = self.CUSTOM_PAYLOAD_CLASS  # need to alias so ignore works below
            return c(**custom_payload_args)  # type: ignore [call-arg]

    def unpack_custom_payload_to_payloads(
        self,
        custom_payload: CPCT,
        endianness_code: str = ENDIANNESS_CODE
    ) -> EnhancedPayloadCollection:
        """Unpacks the given `custom_payload` object into an
        `EnhancedPayloadCollection`.

        NOTE: This does not automatically replace `payloads`, though you could
        do that by `self.payloads=self.unpack_custom_payload_to_payloads(...)`
        if desired.
        """
        # Autopopulate payloads based on name:
        payloads: EnhancedPayloadCollection = EnhancedPayloadCollection()

        # Go through every telemetry channel in the linked `DataStandards`
        # `Module` and lookup the value in the Custom Payload.
        module = self.get_ds_module()
        for channel in module.telemetry.vals:
            try:
                payloads.append(TelemetryPayload(
                    module_id=module.ID,
                    channel_id=channel.ID,
                    data=getattr(custom_payload, channel.name),
                    # TODO: NOTE, it should be made clear that for payloads inside a `CustomPayloadPacket` `timestamp` represents an Earth (unpacking) time and not a Rover (generation) time
                    timestamp=int(time.time()),
                    magic=Magic.TELEMETRY,
                    endianness_code=endianness_code
                ))
            except AttributeError:
                raise NotImplementedError(
                    f"`CustomPayloadPacket` class {self.__class__} was given "
                    f"a `CustomPayload` of class {custom_payload.__class__} "
                    f"which should contain one attribute (as a `__dict__` "
                    "entry, `__slots__` entry, or computed `@property`) for "
                    "each `TelemetryChannel` in the given special `DataStandards` "
                    f"module: `{module}` *but* it is missing at least "
                    f"one: there is no attribute with the same name as the "
                    f"`TelemetryChannel`: {channel} in the `CustomPayload` "
                    f"class {custom_payload.__class__}."
                )

        return payloads

    def __init__(self,
                 custom_payload: Optional[CPCT] = None,
                 payloads: Optional[EnhancedPayloadCollection] = None,
                 raw: Optional[bytes] = None,
                 endianness_code: str = ENDIANNESS_CODE
                 ) -> None:
        """ Constructs an instance of this `CustomPayloadPacket`. If a
        `custom_payload` (`CPCT`) object is given (e.g. when decoding), it will
        be unpacked into a `EnhancedPayloadCollection`. If a `payloads`
        `EnhancedPayloadCollection` is given, it will just be loaded.
        NOTE: `custom_payload` XOR `payloads` must be given. Not neither, not
        both.
        """

        if custom_payload is None and payloads is None:
            raise ValueError(
                "The constructor of `CustomPayloadPacket` class "
                f"{self.__class__} can only be given `custom_payload` XOR "
                "`payloads`, not neither and not both. Neither were given. "
                "Please provide at least one."
            )
        if custom_payload is not None and payloads is not None:
            raise ValueError(
                "The constructor of `CustomPayloadPacket` class "
                f"{self.__class__} can only be given `custom_payload` XOR "
                "`payloads`, not neither and not both. Both were given. "
                "Please provide only one. "
                f"`custom_payload` was `{custom_payload}`. "
                f"`payloads` was `{payloads}`. "
            )

        if custom_payload is not None:
            payloads = self.unpack_custom_payload_to_payloads(
                custom_payload=custom_payload,
                endianness_code=endianness_code
            )
            # Then store these in the instance right away (so super's `__init__`
            # doesn't trigger an unnecessary recompute of `_custom_payload`
            # when it calls the `payloads` setter):
            self._custom_payload = custom_payload
            # store `_payloads` automatically too to prevent the recompute (see the `payloads` setter for more notes.)
            self._payloads = payloads
        else:
            # we now implicitly know `payloads` is not `None` but `mypy`
            # doesn't realize it yet. So, let it know:
            payloads = cast(EnhancedPayloadCollection, payloads)
            # NOTE: don't store to `_payloads` straight away since we don't
            # know `_custom_payload`, so we'll want `super` to trigger a
            # recompute when it calls the `payloads=` setter.

        super().__init__(
            payloads=payloads,
            raw=raw,
            endianness_code=endianness_code
        )
