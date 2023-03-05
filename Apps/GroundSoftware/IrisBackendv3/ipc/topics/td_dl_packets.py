from typing import Final, List

from IrisBackendv3.ipc.topic_definition import TopicDefinition
from IrisBackendv3.ipc.messages import DownlinkedPacketsMessage
from IrisBackendv3.codec.packet import Packet

TD_DL_PACKETS: Final = TopicDefinition(
    topic_type=List[Packet],
    message_def=DownlinkedPacketsMessage
)
