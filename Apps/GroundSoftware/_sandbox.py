from IrisBackendv3.ipc.messages import DownlinkedPacketsMessage, DownlinkedPacketsContent
import IrisBackendv3 as IB3
IB3.init_from_latest()

xcvr = IB3.transceiver.prebuilts.build_xcvr_by_name(
    'PCAP-18H',
    packetgap=37000,  # skip first 37000 packets (of 37644)
    fixed_period_ms=1000,
    loop=True,
    log_on_receive=False
)
xcvr.begin()

packets = xcvr.read()

dpm = DownlinkedPacketsMessage(DownlinkedPacketsContent(
    packets=packets
))
dpmb = dpm.to_ipc_bytes()
print(dpmb)

dpm2 = DownlinkedPacketsMessage.from_ipc_bytes(dpmb)

assert (dpm == dpm2)
