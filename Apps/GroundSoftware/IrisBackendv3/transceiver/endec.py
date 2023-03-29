# -*- coding: utf-8 -*-

"""
Classes to encoders/decoders (`Endec`s) to convert bytes for `Transceiver`
messages.

Using instantiatable classes and not namespaces or singletons since some
`Endec`s may be need to be stateful.

NOTE: Technically speaking, there's no difference between an `endec` and a
`codec` but the name `Endec` is used here to avoid confusion with the
`CodecContainers` in the `codec` package used to encode/decode Iris Packets
themselves. These `Endec`s are just used for wrapping and unwrapping data sent
through a `Transceiver`.

NOTE: All `Endec`s expect there to be **exactly one** packet's worth of
information in the supplied data.

@author: Connor W. Colombo (CMU)
@last-updated: 05/12/2022
"""

from IrisBackendv3.transceiver.endecs.endec import Endec
from IrisBackendv3.transceiver.endecs.unity_endec import UnityEndec
from IrisBackendv3.transceiver.endecs.ip_udp_endec import IpUdpEndec
from IrisBackendv3.transceiver.endecs.slip_endec import SlipEndec