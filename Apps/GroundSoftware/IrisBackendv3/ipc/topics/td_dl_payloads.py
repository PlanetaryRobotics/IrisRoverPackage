"""
Unlike `TD_DL_PACKETS`, these payloads may or may not have come
**directly** from a packet (they could have also come from a MetaChannel which
generated extra payloads based on what it's read from packet payloads).

Listen for `DownlinkedPacketsMessage` and read `packet.payloads` for each
`message.content.packets` if you want just payloads that came from packets.

@author: Connor W. Colombo (CMU)
@last-updated: 03/06/2023
"""

from typing import Final

from IrisBackendv3.ipc.topic_definition import TopicDefinition
from IrisBackendv3.ipc.messages import DownlinkedPayloadsMessage
from IrisBackendv3.codec.packet_classes.packet import Packet

TD_DL_PAYLOADS: Final = TopicDefinition(
    message_def=DownlinkedPayloadsMessage
)
