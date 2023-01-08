"""
Prototype BIL used by ARTEMIS. Test case for the main BIL.

@author: Connor W. Colombo (CMU)
@last-updated: 01/07/2023
"""

from typing import Final, Optional

from concurrent import futures
import logging

import multiprocessing

import grpc
import built_protos.artemis_bil_pb2 as pb2
import built_protos.artemis_bil_pb2_grpc as pb2_grpc

from opts import get_opts
import db_wrapper as DB

N_WORKERS: Final[int] = (multiprocessing.cpu_count() + 1) // 2


class ARTEMIS_BIL(pb2_grpc.BilServicer):
    db_client: DB.Client

    def SendTelem(self, request, context):
        """Asks the BIL to send telem to the rest of the stack:
        """
        logging.info(f"Got Telem: {request}")


def serve(port: int):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=N_WORKERS))
    pb2_grpc.add_BilServicer_to_server(ARTEMIS_BIL(), server)
    server.add_insecure_port('[::]:' + port)
    server.start()
    print("Server started, listening on " + port)
    server.wait_for_termination()


if __name__ == '__main__':
    logging.basicConfig()
    opts = get_opts()

    # Start the BIL server:
    serve(port=opts.port)
