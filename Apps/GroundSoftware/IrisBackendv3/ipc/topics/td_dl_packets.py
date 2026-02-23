from typing import Final

from IrisBackendv3.ipc.topic_definition import TopicDefinition
from IrisBackendv3.ipc.messages import DownlinkedPacketsMessage
from IrisBackendv3.codec.packet_classes.packet import Packet

TD_DL_PACKETS: Final = TopicDefinition(
    message_def=DownlinkedPacketsMessage
)
