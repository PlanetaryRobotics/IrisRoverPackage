import asyncio
import traceback
from typing import cast, Final, List

from IrisBackendv3.ipc.messages import DownlinkedPacketsMessage, DownlinkedPacketsContent
from IrisBackendv3.ipc.logging import logger
import IrisBackendv3 as IB3
IB3.init_from_latest()

PERIOD_MS: Final[float] = 10

xcvr = IB3.transceiver.prebuilts.build_xcvr_by_name(
    'PCAP-18H',
    packetgap=37000,  # skip first 37000 packets (of 37644)
    fixed_period_ms=PERIOD_MS,
    loop=False,
    log_on_receive=False
)
xcvr.begin()


async def check_all_packets() -> None:
    try:
        while len(packets := await xcvr.async_read()) != 0:
            dpm = DownlinkedPacketsMessage(DownlinkedPacketsContent(
                packets=packets
            ))

            dpmb = dpm.to_ipc_bytes()
            dpm2 = DownlinkedPacketsMessage.from_ipc_bytes(dpmb)
            print(dpm2)

            assert (dpm == dpm2)
    except Exception as e:
        print(e)
        dpmb = dpm.to_ipc_bytes()
        dpm2 = DownlinkedPacketsMessage.from_ipc_bytes(dpmb)
        from IrisBackendv3.codec.payload import TelemetryPayload
        from IrisBackendv3.ipc.serializer import serialize, deserialize
        packets = dpm.content.packets
        tt = [t for pkt in packets for t in pkt.payloads[TelemetryPayload]]
        telem = cast(List[TelemetryPayload], tt)
        try:
            for t in telem:
                print(t)
                serialize(t)
        except Exception as ee:
            print(t, t.data)
            print(ee)


async def ticker() -> None:
    # some other random task to prove concurrency
    while True:
        await asyncio.sleep(PERIOD_MS/1000/2)
        print("\tTick")


async def run() -> None:
    tasks = [
        asyncio.create_task(check_all_packets(), name="Check"),
        asyncio.create_task(ticker(), name="Tick")
    ]

    # Clean up if the above closes for some reason:
    try:
        done, pending = await asyncio.wait(
            tasks,
            timeout=None,
            return_when=asyncio.FIRST_COMPLETED
        )

        # We're done. Log results:
        for task in done:
            result, exception, trace = None, None, None
            try:
                result = task.result()
            except Exception as e:
                exception = e
                trace = '\n'.join(traceback.format_tb(e.__traceback__))
            logger.notice(
                f"Task {task.get_name()} ended "
                f"with `{result=}`, `{exception=}`,\n"
                f"`trace={trace}`."
            )
        for task in pending:
            task.cancel()
            logger.notice(
                f"Task {task.get_name()} didn't end. Now cancelled."
            )
    except Exception as e:
        logger.error(f"Exception occurred while handling: \n\t{e}.")

asyncio.run(run())
