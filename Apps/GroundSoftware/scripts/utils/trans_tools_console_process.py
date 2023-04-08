"""
The actual multiprocessing that runs `trans_tools_console`.

@author: Connor W. Colombo (CMU)
@last-updated: 09/17/2022
"""
import pynput  # type: ignore
import asyncio
import threading
import socketserver
import serial_asyncio  # type: ignore
from dataclasses import dataclass
from multi_await import multi_await  # type: ignore
from serial.tools import list_ports, list_ports_common  # type: ignore
from termcolor import colored

from scripts.utils.trans_tools import *
from IrisBackendv3.codec.payload import EventPayload

import scapy.all as scp  # type: ignore

# Async Serial Writer Handle:
serial_writer: Optional[asyncio.StreamWriter] = None

# Application Context (settings, flags, etc. to be passed by reference to functions that need it):
app_context: Dict[str, Any] = {
    # Whether or not to hide the command table (False = show the table):
    'collapse_command_table': False,
    # Whether to print raw SLIP bytes received for all packets to the console:
    'echo_all_packet_slip': False,
    # Whether to print bytes received for all packets to the console:
    'echo_all_packet_bytes': False,
    # WiFi Networking Settings:
    'wifi_settings': {
        'rover_ip': '192.168.150.3',
        'rover_port': 42000,
        'gateway_ip': '192.168.150.254',
        'lander_ip': '192.168.10.105',
        'lander_port': 43531,
    }
}

# Special message types that wrap primitives (so we can check type in Queue receiver):


@dataclass
class QueueMessage:
    msg: str


@dataclass
class QueueTick:
    count: int


@dataclass
class QueueByte:
    byte: bytes


def stream_keys(key_queue, message_queue):
    # Start a keyboard listener that streams keypresses into
    # the given Queue `q`:
    loop = asyncio.get_event_loop()

    def on_press(key):
        # this callback is invoked from another thread, so we can't
        # just queue.put_nowait(key), we have to go through
        # call_soon_threadsafe
        loop.call_soon_threadsafe(key_queue.put_nowait, key)
    listener = pynput.keyboard.Listener(on_press=on_press)
    listener.start()

    if hasattr(listener, 'IS_TRUSTED') and not listener.IS_TRUSTED:
        message_queue.put_nowait(QueueMessage(
            f"Keyboard listener might not have appropriate permissions. "
            f"If you see this warning and the keyboard input doesn't work, "
            f"Run as sudo and make sure to whitelist the application in "
            f"`Enable access for assistive devices` if on MacOS, this may mean "
            f"whitelisting both the terminal and python/pyenv."
        ))


def make_udp_handler(app_context: Dict[str, Any], packet_queue, message_queue):
    # Make a UDPHandler class with access to the app_context.

    # class DatagramRequestHandler(BaseRequestHandler):

    #     """Define self.rfile and self.wfile for datagram sockets."""

    #     def setup(self):
    #         from io import BytesIO
    #         self.packet, self.socket = self.request
    #         self.rfile = BytesIO(self.packet)
    #         self.wfile = BytesIO()
    class IrisConsoleUDPHandler(socketserver.BaseRequestHandler):
        def setup(self):
            from io import BytesIO
            self.packet, self.socket = self.request
            self.rfile = BytesIO(self.packet)
            self.wfile = BytesIO()

        def finish(self):
            # Nothing to do. We could send an ack back or something which would look like:
            # self.socket.sendto(self.wfile.getvalue(), self.client_address)
            pass

        def handle(self):
            src_ip, src_port = self.client_address
            # Make sure src IP is the rover or the gateway:
            if (src_ip not in [
                app_context['wifi_settings']['rover_ip'],
                app_context['wifi_settings']['gateway_ip']
            ]):
                message_queue.put_nowait(QueueMessage(
                    f"Got bad datagram IP in WiFi UDP packet: {src_ip}"
                ))

            # Make sure src port is the rover or lander reflected by the gateway:
            rover_port = app_context['wifi_settings']['rover_port']
            if (src_port not in [
                rover_port,
                rover_port+1,  # A special testing case
                app_context['wifi_settings']['lander_port']
            ]):
                message_queue.put_nowait(QueueMessage(
                    f"Got bad datagram port in WiFi UDP packet: {src_port}"
                ))

            # Grab the UDP data bytes:
            data_bytes = self.request[0]

            try:
                # Process it:
                packet = parse_packet(data_bytes)

                # Set pathway packet (and all its payloads):
                packet.pathway = DataPathway.WIRELESS
                packet.source = DataSource.WIFI_CONNECTION_DIRECT

                # Push it into the queue:
                packet_queue.put_nowait(packet)

            except PacketDecodingException as pde:
                # Print what happened:
                msg = (
                    "While streaming data, a PacketDecodingException occurred. The data "
                    "bytes at the time of the exception were: \n"
                    f"{scp.hexdump(data_bytes, dump=True)}\n."
                    f"The PacketDecodingException was: `{pde}`."
                    # In data view, just push the string to the packet print console:
                )
                message_queue.put_nowait(QueueMessage(msg))
            except Exception as e:
                msg = f"An otherwise unresolved error occurred during packet streaming: {e}"
                # In data view, just push the string to the packet print console:
                message_queue.put_nowait(QueueMessage(msg))

    return IrisConsoleUDPHandler


class BasicSlipTransceiver():
    escape: bool
    keep_running: bool
    nrx: int
    line: bytes
    data_bytes: bytearray
    slip_state: SlipState

    # Queue to push bytes into:
    packet_queue: asyncio.Queue
    # Queue to push non-telemetry messages into:
    message_queue: asyncio.Queue

    def __init__(self,
                 packet_queue: asyncio.Queue,
                 message_queue: asyncio.Queue
                 ) -> None:
        self.packet_queue = packet_queue
        self.message_queue = message_queue

        # Init state:
        self.escape: bool = False
        self.keep_running: bool = True
        self.nrx: int = 0
        self.line: bytes = b''
        self.data_bytes: bytearray = bytearray(b'')
        self.slip_state: SlipState = SlipState.FIRST_END

    def append_byte(self, b: int) -> None:
        if self.escape:
            if b == 0xDC:
                self.data_bytes.append(0xC0)
            elif b == 0xDD:
                self.data_bytes.append(0xDB)
            self.escape = False
        else:
            if b == 0xDB:
                self.escape = True
            else:
                self.data_bytes.append(b)

    # Returns if we just completed a packet
    def parse_new_byte(self, newByte: bytes) -> bool:
        madeAPacket: bool = False
        try:
            self.line += newByte
            b = int.from_bytes(newByte, 'big')

            if self.slip_state == SlipState.FIRST_END:
                if b == 0xC0:
                    self.slip_state = SlipState.FIRST_BYTE_OR_STARTING_END
            elif self.slip_state == SlipState.FIRST_BYTE_OR_STARTING_END:
                if b != 0xC0:
                    self.append_byte(b)
                    self.slip_state = SlipState.STARTED
            elif self.slip_state == SlipState.STARTED:
                if b == 0xC0:
                    if len(self.data_bytes) != 0:  # packet baked:
                        # Process it:
                        packet = parse_ip_udp_packet(self.data_bytes)
                        # Set pathway for packet (and all its payloads):
                        packet.pathway = DataPathway.WIRED
                        packet.source = DataSource.UDP_SERIAL_DIRECT
                        # Push it into the queue:
                        self.packet_queue.put_nowait(packet)
                        # Move on:
                        self.data_bytes = bytearray(b'')
                        self.slip_state = SlipState.FIRST_BYTE_OR_STARTING_END
                        madeAPacket = True
                else:
                    self.append_byte(b)

        except KeyboardInterrupt:
            self.data_bytes = bytearray(b'')
            self.slip_state = SlipState.FIRST_BYTE_OR_STARTING_END
        except PacketDecodingException as pde:
            # Reset SLIP:
            self.data_bytes = bytearray(b'')
            self.slip_state = SlipState.FIRST_BYTE_OR_STARTING_END

            # Print what happened:
            msg = (
                "While streaming data, a PacketDecodingException occurred. The data "
                "bytes at the time of the exception were: \n"
                f"{scp.hexdump(self.data_bytes, dump=True)}\n."
                f"The PacketDecodingException was: `{pde}`."
                # In data view, just push the string to the packet print console:
            )
            self.message_queue.put_nowait(QueueMessage(msg))

        except Exception as e:
            msg = f"An otherwise unresolved error occurred during packet streaming: {e}"
            # In data view, just push the string to the packet print console:
            self.message_queue.put_nowait(QueueMessage(msg))
            self.data_bytes = bytearray(b'')
            self.slip_state = SlipState.FIRST_BYTE_OR_STARTING_END

        return madeAPacket


async def stream_SLIP_packets(serial_settings, packet_queue, message_queue, app_context):
    global serial_writer
    # Stream packets into the given Queue `packet_queue` from bytes received over
    # SLIP on the serial interface defined by the given `serial_settings`.
    # Any error (etc.) messages that come up are put into `message_queue`.

    # Log of all slip byte received in this packet:
    slip_packet_bytes_log: bytes = b''

    # # Connect to echo-out serial:
    # if app_context['echo_all_packet_bytes']:
    #     try:
    #         out_serial_device = [cp.device for cp in list_ports.comports() if cp.product=='UPort 1150'][0]
    #         out_ser = serial.Serial(out_serial_device, serial_settings['baud'])  # open serial port
    #         message_queue.put_nowait(QueueMessage("Echo Out Serial Connection Success!"))
    #     except serial.SerialException as e:
    #         message_queue.put_nowait(QueueMessage(
    #             f"Failed to connect to echo-out serial device. Is the device name right? Check the connection? Original error: {e}"
    #         ))

    # Connect to Serial:
    try:
        serial_device_id = [cp.device for cp in list_ports.comports(
        ) if cp.serial_number is not None and serial_settings['device_sn'] in cp.serial_number][0]
        reader, writer = await serial_asyncio.open_serial_connection(url=serial_device_id, baudrate=serial_settings['baud'])
        serial_writer = writer
        message_queue.put_nowait(QueueMessage(
            "Main Serial Connection Success!"))
    except Exception as e:
        message_queue.put_nowait(QueueMessage(
            f"Failed to connect to main serial device. Is the device name right? Check the connection? Original error: {e}"
        ))
        return  # Don't continue

    # Fire up the Transceiver:
    slip_xcvr = BasicSlipTransceiver(packet_queue, message_queue)

    # Parse serial data as it arrives:
    while True:
        newByte = await reader.readexactly(1)
        madeAPacket = slip_xcvr.parse_new_byte(newByte)
        # Add to log:
        slip_packet_bytes_log += newByte

        # # Echo to output:
        # out_ser.write(newByte)

        if madeAPacket:
            if app_context['echo_all_packet_slip']:
                # If we made a packet with this new byte, push it to the console:
                message_queue.put_nowait(QueueMessage(
                    "Raw Undecoded SLIP:\n-----------\n"+scp.hexdump(slip_packet_bytes_log, dump=True)))
            # Reset log no matter what (packet's done):
            slip_packet_bytes_log = b''


async def ticker(tick_period_s: int, tick_queue):
    # Makes one tick every `tick_period_s` seconds.
    # Puts the current tick count into `tick_queue`.
    tick_count = 0
    while True:
        await asyncio.sleep(tick_period_s)
        tick_count += 1
        tick_queue.put_nowait(QueueTick(tick_count))


async def run_forever():
    # Does nothing, just keeps stuff open.
    while True:
        await asyncio.sleep(1)


def handle_async_packet(packet: Packet) -> None:
    """Handles a packet pushed asynchronously to a queue."""
    # If there are any EventPayloads in the packet,
    # print them in the order received (NOTE: some
    # don't have timestamps - e.g. RadioGround - so
    # sorting by time would be hairy):
    events = [*packet.payloads[EventPayload]]
    for event in events:
        # Push directly to the queue:
        # ... the handle_streamed_packet will take care of the refreshing
        nontelem_packet_prints.appendleft(
            f"\033[35;47;1m({datetime.now().strftime(DATETIME_FORMAT_STR)})\033[0m {event!s}"
        )

    if app_context['echo_all_packet_bytes']:
        # Echo the data out...
        echo_bytes: bytes = packet.raw if packet.raw is not None else b''
        prefix = ''
        if packet.pathway == DataPathway.WIRED:
            prefix = 'RS422 '
        elif packet.pathway == DataPathway.WIRELESS:
            prefix = 'WIFI '
        # To the console:
        nontelem_packet_prints.appendleft(
            f"{prefix}Packet Bytes:\n-----------------\n" +
            scp.hexdump(echo_bytes, dump=True)
        )

    # NOTE: this refreshes the screen too
    handle_streamed_packet(packet, True, app_context)


def handle_async_message(msg: str) -> None:
    """Handles a message pushed asynchronously to a queue."""
    # Add the message to the non-telemetry feed:
    nontelem_packet_prints.appendleft(msg)
    # Refresh the display:
    refresh_console_view(app_context)


async def console_main(serial_settings):
    global serial_writer
    # Main console drawing process that streams packets, bytes, and error messages and updates the screen whenever something comes in.
    # Uses the serial interface defined by the given settings.

    # Set up the console:
    init_console_view()

    # Setup IPC Queues:
    packet_queue = asyncio.Queue()
    message_queue = asyncio.Queue()
    key_queue = asyncio.Queue()
    tick_queue = asyncio.Queue()

    # Start WiFi UDP Server:
    server_active: bool
    try:
        udp_server = socketserver.ThreadingUDPServer(
            (
                app_context['wifi_settings']['lander_ip'],
                app_context['wifi_settings']['lander_port']
            ),
            make_udp_handler(app_context, packet_queue, message_queue),
            bind_and_activate=True
        )
        server_active = True
        message_queue.put_nowait(QueueMessage(
            colored("UDP Server Bound.", 'green')))
    except Exception as e:
        message_queue.put_nowait(QueueMessage(
            colored("UDP Server Failed to Bind.", 'red')))
        server_active = False

        class DummyWithable__UdpConnectionFailed():
            def __init__(self): pass
            def __enter__(self, *args, **kwargs): pass
            def __exit__(self, *args, **kwargs): pass
        # Basically `None` but supports `with`
        udp_server = DummyWithable__UdpConnectionFailed()

    # Add pointer to context:
    app_context['udp_server'] = udp_server
    with udp_server:
        if server_active:
            udp_server_thread = threading.Thread(
                target=udp_server.serve_forever)
            udp_server_thread.daemon = True
            udp_server_thread.start()

        # Start tasks:
        stream_keys(key_queue, message_queue)
        tasks = [
            asyncio.create_task(stream_SLIP_packets(
                serial_settings, packet_queue, message_queue, app_context)),
            # this controls the base draw rate of the screen (i.e. how frequently we update if no packets or key presses are received)
            asyncio.create_task(ticker(1, tick_queue))
        ]

        async with multi_await() as m:
            m.add(packet_queue.get)
            m.add(message_queue.get)
            m.add(key_queue.get)
            m.add(tick_queue.get)
            m.add(run_forever)  # Does nothing but keep things open

            done = False
            while not done:
                results, failures = await m.get()

                assert failures == [None] * len(m.srcs)

                for r in results:
                    if r is not None:
                        if isinstance(r, Packet):
                            handle_async_packet(cast(Packet, r))
                        elif isinstance(r, (pynput.keyboard.Key, pynput.keyboard.KeyCode)):
                            # NOTE: this refreshes the screen too
                            handle_keypress(r, message_queue,
                                            serial_writer, app_context)
                        elif isinstance(r, str):
                            handle_async_message(cast(str, r))
                        elif isinstance(r, QueueMessage):
                            handle_async_message(cast(QueueMessage, r).msg)
                        elif isinstance(r, QueueTick):
                            # Don't do anything special besides update screen:
                            refresh_console_view(app_context)

        # Clean up if the above closes for some reason:
        [await t for t in tasks]

        # Close the UDP Server:
        if server_active:
            udp_server_thread.shutdown()


def start_console(serial_device_sn: str = 'A7035PDL', baud: int = 57600):
    # Starts the console, connecting to a serial device with the given serial number at the given baud rate.

    # Build serial settings struct to pass around:
    serial_settings = {
        'device_sn': serial_device_sn,
        'baud': baud
    }

    # Start the main process:
    asyncio.run(console_main(serial_settings))
