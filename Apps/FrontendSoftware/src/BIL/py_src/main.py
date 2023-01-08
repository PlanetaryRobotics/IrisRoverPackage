"""
Root of the Iris Frontend's Backend Interface Layer (BIL).

The BIL is meant to be bundled as an executable which is launched by the
Electron backend. Main communication between the two will occur over `gRPC`
according to the API specified in `bil.proto`. However, DB connection info will
be supplied via the shell and the BIL application will respond via the shell
with connection success/fail and the actual port gRPC will use (in case of
conflict) once the DB is connected and gRPC is ready.

If DB connection fails fatally (e.g. due to no network connection), the BIL
will need to be restarted.

DB connection data is sent once over the shell interface because:
1. The BIL can't do anything else meaningful until then.
2. We'd have to add security to make it safe to send over gRPC, since then they
    could be exposed to the network.

@author: Connor W. Colombo (CMU)
@last-updated: 01/07/2023
"""

from typing import Final, Optional

from concurrent import futures
import logging

import multiprocessing

import grpc
import built_protos.bil_pb2 as pb2
import built_protos.bil_pb2_grpc as pb2_grpc

from opts import get_opts
import db_wrapper as DB

N_WORKERS: Final[int] = (multiprocessing.cpu_count() + 1) // 2


class BIL(pb2_grpc.BilServicer):
    db_client: DB.Client

    def __init__(self, db_client: DB.Client):
        super().__init__()
        self.db_client = db_client

    def Ping(self, request, context):
        """Aliveness Ping (for debugging):
        """
        return pb2.PingReply(request.payload)

    def SendCommand(self, request, context):
        """Tells the BIL to push the given command to the database:
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def RequestStoredData(self, request, context):
        """Generic one-time request for stored data
        (generic because any received data goes straight into an event emitter):
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def StreamNewData(self, request, context):
        """Stream all new data that matches the given request:
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def serve(client: DB.Client, port: int):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=N_WORKERS))
    bil = BIL(client)
    pb2_grpc.add_BilServicer_to_server(bil, server)
    server.add_insecure_port('[::]:' + port)
    server.start()
    print("Server started, listening on " + port)
    server.wait_for_termination()


if __name__ == '__main__':
    logging.basicConfig()
    opts = get_opts()

    # Connect to client.
    try:
        client = DB.connect_client(
            protocol=opts.protocol,
            server=opts.server,
            user=opts.user_name,
            code=opts.code
        )
        logging.info(
            f"[DBC-SUCC]: DB client connected successfully "
            f"to {opts.server} as {opts.user_name}."
        )
    except Exception as e:
        # Log the error:
        logging.error(
            f"[DBC-FAIL]: DB failed to connect to "
            f"{opts.server} as {opts.user_name}, "
            f"because: {e}."
        )
        # Then reraise the exception:
        # If this fails, we want to halt the BIL; so, it's fine that this has
        # an unhandled exception, since that will get caught as the
        # `stderr` level by whatever is calling the BIL (Electron).
        raise e

    # Start the BIL server:
    serve(
        client=client,
        port=opts.port
    )
