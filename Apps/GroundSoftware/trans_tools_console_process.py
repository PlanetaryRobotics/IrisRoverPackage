"""
The actual multiprocessing that runs `trans_tools_console`.

@author: Connor W. Colombo (CMU)
@last-updated: 09/06/2022
"""
import pynput
import asyncio
import serial_asyncio
from dataclasses import dataclass
from multi_await import multi_await
from serial.tools import list_ports, list_ports_common

from trans_tools import *

import scapy.all as scp

# Async Serial Writer Handle:
serial_writer = None

# Application Context (settings, flags, etc. to be passed by reference to functions that need it):
app_context: Dict[str, Any] = {
    # Whether or not to hide the command table (False = show the table):
    'collapse_command_table': False,
    # Whether to print raw SLIP bytes received for all packets to the console:
    'echo_all_packet_slip': False,
    # Whether to print bytes received for all packets to the console:
    'echo_all_packet_bytes': False
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

def stream_keys(q):
    # Start a keyboard listener that streams keypresses into
    # the given Queue `q`:
    loop = asyncio.get_event_loop()
    def on_press(key):
        # this callback is invoked from another thread, so we can't
        # just queue.put_nowait(key), we have to go through
        # call_soon_threadsafe
        loop.call_soon_threadsafe(q.put_nowait, key)
    pynput.keyboard.Listener(on_press=on_press).start()

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

    def parse_new_byte(self, newByte: bytes) -> bool: # Returns if we just completed a packet
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
                    "While streaming data, a PacketDecodingException occured. The data "
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
        serial_device_id = [cp.device for cp in list_ports.comports() if cp.serial_number is not None and serial_settings['device_sn'] in cp.serial_number][0]
        reader, writer = await serial_asyncio.open_serial_connection(url=serial_device_id, baudrate=serial_settings['baud'])
        serial_writer = writer
        message_queue.put_nowait(QueueMessage("Main Serial Connection Success!"))
    except Exception as e:
        message_queue.put_nowait(QueueMessage(
            f"Failed to connect to main serial device. Is the device name right? Check the connection? Original error: {e}"
        ))

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
                message_queue.put_nowait(QueueMessage("Raw Undecoded SLIP:\n-----------\n"+scp.hexdump(slip_packet_bytes_log, dump=True)))
            # Reset log no matter what (packet's done):
            slip_packet_bytes_log = b''


async def ticker(tick_period_s: int, tick_queue):
    # Makes one tick every `tick_period_s` seconds.
    # Puts the current tick count into `tick_queue`.
    tick_count = 0
    while True:
        await asyncio.sleep(tick_period_s)
        tick_count += 1
        await tick_queue.put(QueueTick(tick_count))

async def run_forever():
    # Does nothing, just keeps stuff open.
    while True:
        await asyncio.sleep(1)

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

    # Start tasks:
    stream_keys(key_queue)
    tasks = [
        asyncio.create_task(stream_SLIP_packets(serial_settings, packet_queue, message_queue, app_context)),
        asyncio.create_task(ticker(1, tick_queue)) # this controls the base draw rate of the screen (i.e. how frequently we update if no packets or key presses are received)
    ]

    async with multi_await() as m:
        m.add(packet_queue.get)
        m.add(message_queue.get)
        m.add(key_queue.get)
        m.add(tick_queue.get)
        m.add(run_forever) # Does nothing but keep things open

        done = False
        while not done:
            results, failures = await m.get()

            assert failures == [None] * len(m.srcs)
            
            for r in results:
                if r is not None:
                    if isinstance(r, Packet):
                        packet = cast(Packet, r)
                        handle_streamed_packet(packet, True, app_context) # NOTE: this refreshes the screen too

                        if app_context['echo_all_packet_bytes']:
                            # Echo the data out...
                            echo_bytes: bytes = packet.raw if packet.raw is not None else b''
                            # To the console:
                            message_queue.put_nowait(QueueMessage("Packet Bytes:\n-----------\n"+scp.hexdump(echo_bytes, dump=True)))

                    elif isinstance(r, (pynput.keyboard.Key, pynput.keyboard.KeyCode)):
                        handle_keypress(r, message_queue, serial_writer, app_context) # NOTE: this refreshes the screen too
                    elif isinstance(r, str):
                        msg = cast(str, r)
                        nontelem_packet_prints.appendleft(msg)
                        refresh_console_view(app_context)
                    elif isinstance(r, QueueMessage):
                        msg = cast(QueueMessage, r).msg
                        # Add the message to the non-telemetry feed:
                        nontelem_packet_prints.appendleft(msg)
                        # Refresh the display:
                        refresh_console_view(app_context)
                    elif isinstance(r, QueueTick):
                        # Don't do anything special besides update screen:
                        refresh_console_view(app_context)

    # Clean up if the above closes for some reason:
    [await t for t in tasks]

def start_console(serial_device_sn: str = 'A7035PDL', baud: int = 57600):
    # Starts the console, connecting to a serial device with the given serial number at the given baud rate.

    # Build serial settings struct to pass around:
    serial_settings = {
        'device_sn': serial_device_sn,
        'baud': baud
    }

    # Start the main process:
    asyncio.run(console_main(serial_settings))