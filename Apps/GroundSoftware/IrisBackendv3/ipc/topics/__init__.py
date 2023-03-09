# Expose all `TopicDefinition`s from the `td_` files in this directory:
# They used to be more but now most of these files are just one-liners with
# imports but, hypothetically, they could support special config or maybe even
# adaptations to message defs.

# Use absolute imports to ensure proper placement in `sys.modules`:
from IrisBackendv3.ipc.topics.td_dl_packets import TD_DL_PACKETS
from IrisBackendv3.ipc.topics.td_dl_payloads import TD_DL_PAYLOADS
from IrisBackendv3.ipc.topics.td_ul_packet import TD_UL_PACKET
from IrisBackendv3.ipc.topics.td_ul_payloads import TD_UL_PAYLOADS
