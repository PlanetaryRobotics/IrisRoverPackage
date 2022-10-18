import IrisBackendv3.codec.bgapi as bgapi

bgapi_lib = bgapi.BGLibListener(bgapi.SerialConnector(
    '/dev/cu.usbserial-A50285BI'), bgapi.BGAPI_WIFI_DEF, listening_to_host=False)

bgapi_lib.open()


async def main():
    for e in bgapi_lib.gen_events(timeout=None, max_time=5):
        print(f"\nReceived {e.__class__.__name__}: {repr(e)}")

if __name__ == "__main__":
    import asyncio
    asyncio.run(main())
