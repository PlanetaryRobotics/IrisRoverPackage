from typing import Final

from IrisBackendv3.ipc.topic_definition import TopicDefinition
from IrisBackendv3.ipc.messages import UplinkPayloadsRequestMessage
from IrisBackendv3.codec.packet_classes.packet import Packet

TD_UL_PAYLOADS: Final = TopicDefinition(
    message_def=UplinkPayloadsRequestMessage
)
