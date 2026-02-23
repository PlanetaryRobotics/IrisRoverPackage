# YAMCS Testing Script from Iris Functional Checkout TRR on Aug. 09 2022

from yamcs.client import YamcsClient # type: ignore
from yamcs.core.auth import Credentials as YamcsCredentials  # type: ignore

import time
import argparse

parser = argparse.ArgumentParser(
    description='IRIS Lunar Rover — Data Standards Lookup — CLI')


def get_opts():
    """
    Return settings wrapped in argparse.
    """
    parser.add_argument('-u', '--username', default='iris',
                        help='YAMCS Username')
    parser.add_argument('-p', '--password', default='not-a-real-password',
                        help='YAMCS Password')

    opts = parser.parse_args()
    return opts

def obtain_client(opts) -> YamcsClient:
    credentials = YamcsCredentials(username=opts.username, password=opts.password)
    return YamcsClient("plyamcs1.mission.local:443", credentials=credentials, tls=True, tls_verify=True)

def print_data(data) -> None:
    if data is not None:
        for parameter in data.parameters:
            print(parameter)
    else:
        print('None')

if __name__ == "__main__":
    opts = get_opts()
    client = obtain_client(opts)

    # Exercise non-websocket API:
    mdb = client.get_mdb(instance='Astrobotic-M1')
    params = [p for p in mdb.list_parameters()]
    print(f"Parameters: {[str(p) for p in params]}")

    # Exercise websocket API:
    if len(params) > 0:
        processor = client.get_processor(instance='Astrobotic-M1', processor='realtime')
        print(f"Subscribing to updates of parameter: {params[-1].qualified_name}")
        processor.create_parameter_subscription(params[-1].qualified_name, on_data=print_data)
        time.sleep(30)
        print("Closing.")

