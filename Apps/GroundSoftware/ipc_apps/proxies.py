"""
Brings up all topic proxies necessary for running all IPC apps on all topic
ports as parallel processes.

@author: Connor W. Colombo (CMU)
@last-updated: 04/30/2023
"""

from typing import Final, Tuple, List
from multiprocessing import Process, current_process

import IrisBackendv3.ipc as ipc

# List of all (pub,sub) port tuples to make into proxies:
PROXY_PORTS: Final[List[Tuple[ipc.Port, ipc.Port]]] = [
    (ipc.Port.TRANSCEIVER_PUB, ipc.Port.TRANSCEIVER_SUB),
    # (ipc.Port.CODEC_PUB, ipc.Port.CODEC_SUB),
    # (ipc.Port.STORAGE_PUB, ipc.Port.STORAGE_SUB),
    # (ipc.Port.REMOTEDB_PUB, ipc.Port.REMOTEDB_SUB),
    # (ipc.Port.GUI_PUB, ipc.Port.GUI_SUB),
    # (ipc.Port.ATLAS_PUB, ipc.Port.ATLAS_SUB),
]


def start_proxy(
    inbound_pub_port: ipc.Port,
    outbound_sub_port: ipc.Port
) -> bool:
    proxy = ipc.TopicProxy(
        inbound_port=inbound_pub_port,
        outbound_port=outbound_sub_port,
    )
    proxy.run()
    return True


def main():
    processes = []

    for port_tuple in PROXY_PORTS:
        p = Process(target=start_proxy, args=port_tuple)
        processes.append(p)
        p.start()

    for p in processes:
        p.join()

    return True


if __name__ == "__main__":
    main()
