from collections import OrderedDict
from trans_tools import *
from termcolor import cprint

# TODO: Turn these into unit tests

# Test packets from WD:
tp = [
    b"\xd5\x04\x08\x18\x00\x03\x03\n\x00\x18\x00\x00\x00\x00\x00`\x00B\x80\x0f\xc1\x00\xb8\xf4\x014!\xfd\x0cV\x0e\x18\r\x10'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    b"\xd5\x04\x08\x18\x00\x04\x03\n\x00\x18\x00\x00\x00\x00\x00`\x00B\x80\x0f\xc1\x00\xb8\xf4\x014!\xfd\x0cV\x0e\x18\r\x10'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    b"\xd5\x04\x08\x18\x00\x05\x03\n\x00\x18\x00\x00\x00\x00\x00`\x00B\x80\x0f\xc1\x00\xb8\xf4\x014!\xfd\x0cV\x0e\x18\r\x10'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    b"\xd5\x04\x08\x18\x00\x06\x03\n\x00\x18\x00\x00\x00\x00\x00\xe0\x00B\x80\x0f\xc1\x00\xb8\xf4\x014!\xfd\x0cV\x0e\x18\r\x10'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
]

packet = parse_packet(tp[0])

log_print(packet)

# Fields to test:
# Each field has the following entries: (n_bits, is_adc_val, n_upper_adc_bits, n_lower_adc_bits)
FIELDS_TO_TEST: OrderedDict = OrderedDict([
    ('Io_ChargingStatus1', (1, False, 0, 0)),
    ('Io_ChargingStatus2', (1, False, 0, 0)),
    ('Io_BatteryConnectionStatus', (1, False, 0, 0)),
    ('Io_BatteryLatchStatus', (1, False, 0, 0)),
    ('Io_1V2PowerGood', (1, False, 0, 0)),
    ('Io_1V8PowerGood', (1, False, 0, 0)),
    ('Io_3V3PowerGood', (1, False, 0, 0)),
    ('Io_5V0PowerGood', (1, False, 0, 0)),
    ('Watchdog_State', (8, False, 0, 0)),
    ('Watchdog_DeploymentStatus', (2, False, 0, 0)),
    ('Watchdog_Uart0State', (1, False, 0, 0)),
    ('Watchdog_Uart1State', (1, False, 0, 0)),
    ('Adc_BatteryTempRaw', (12, True, 12, 12)),
    ('Watchdog_DetailedHeartbeatSequenceNumber', (8, False, 0, 0)),
    ('Watchdog_DigitalOutputStates', (32, False, 0, 0)),
    ('Watchdog_ResetLogs', (40, False, 0, 0)),
    ('Adc_LanderVoltageRaw', (7, True, 7, 12)),
    ('Adc_BatteryChargingTempRaw', (9, True, 9, 12)),
    ('Adc_FullSystemVoltageRaw', (5, True, 5, 12)),
    ('Adc_FullSystemCurrentRaw', (9, True, 12, 9)),
    ('Adc_SwitchedBatteryVoltageRaw', (9, True, 9, 12)),
    ('Adc_Vcc24VoltageRaw', (7, True, 7, 12)),
    ('Heater_ControlEnabled', (1, False, 0, 0)),
    ('Heater_IsHeating', (1, False, 0, 0)),
    ('Adc_2V5VoltageRaw', (5, True, 5, 12)),
    ('Adc_2V8VoltageRaw', (5, True, 5, 12)),
    ('Adc_Vcc28VoltageRaw', (6, True, 6, 12)),
    ('Heater_Kp', (16, False, 0, 0)),
    ('Heater_PwmLimit_DutyCycleCounter', (16, False, 0, 0)),
    ('Heater_SetpointValue', (16, False, 0, 0)),
    ('Heater_OnValue', (16, False, 0, 0)),
    ('Heater_OffValue', (16, False, 0, 0)),
    ('Heater_DutyCyclePeriodCycles', (16, False, 0, 0)),
    ('Heater_DutyCycleCounter', (16, False, 0, 0)),
    ('I2C_BatteryChargeRaw', (16, False, 0, 0)),
    ('I2C_BatteryVoltageRaw', (16, False, 0, 0)),
    ('I2C_BatteryCurrentRaw', (16, False, 0, 0)),
    ('I2C_FuelGaugeTempRaw', (16, False, 0, 0)),
    ('I2C_BatteryChargeTelemRaw', (8, False, 0, 0)),
    ('I2C_BatteryCurrentTelemRaw', (8, False, 0, 0))
])

# total # of bytes in detailed status packet
N_PACKET_BYTES: int = math.ceil(sum(n for n, *_ in FIELDS_TO_TEST.values())/8)


def byte_to_binstr(B):
    # Convert to bin string:
    binstr = bin(B)[2:]
    # Pad with appropriate number of zeros:
    binstr = '0' * (8-len(binstr)) + binstr
    return binstr


def bytes_to_binstr(Bs):
    return '0b '+' '.join(byte_to_binstr(b) for b in Bs)


def create_adc_sample(val: int, n_top: int, n_bot: int, n_adc_bits: int = 12):
    # Make `val` at most `n_adc_bits` bits long:
    val = val & ((1 << n_adc_bits)-1)
    # Keep only the top `n_top` bits:
    val = val >> (n_adc_bits-n_top)
    # Keep only the bottom `n_bot` bits:
    val = val & ((1 << n_bot)-1)
    return val


def create_bitfield(test_vals: Dict[str, int]):
    x: int = 0
    idx_bit_head: int = 0
    for field_name, (n_bits, is_adc_val, n_upper_adc_bits, n_lower_adc_bits) in FIELDS_TO_TEST.items():
        field_val = test_vals[field_name]
        if is_adc_val:
            field_val = create_adc_sample(
                field_val, n_upper_adc_bits, n_lower_adc_bits)
        x |= field_val << idx_bit_head
        idx_bit_head += n_bits
    return x.to_bytes(N_PACKET_BYTES, 'little')


# Create a dict with all fields from FIELDS_TO_TEST and a default value of 0:
blank_vals = dict((f, 0) for f in FIELDS_TO_TEST.keys())

print_test_packet_bits: bool = True
idx_bit_head = 0  # index of bit in the test packet at which to start inserting the value
for test_field_name, (test_field_bits, is_adc_val, *_) in [*FIELDS_TO_TEST.items()][:9]:
    idx_byte_head = idx_bit_head // 8  # index of the byte this starts after
    # how many bits into the byte this starts
    idx_bit_head_in_byte = idx_bit_head % 8
    # total number of bytes this value will span
    total_val_bytes_spanned = math.ceil(
        (idx_bit_head_in_byte + test_field_bits)/8)
    print(
        f"Testing field {test_field_name} with {test_field_bits}b at {idx_bit_head=} -> {idx_byte_head}B+{idx_bit_head_in_byte}b")

    # Make a test packet for all possible values for the field:
    if is_adc_val:
        max_test_val = 1 << 12  # go up to ADC max num of bits even if field truncates
    else:
        max_test_val = 1 << test_field_bits
    for test_val in range(0, max_test_val):
        # Start with a packet of all zeros:
        # Create a dict with all fields 0 except `test_field_name`, which has the specified test value:
        test_vals = {**blank_vals, test_field_name: test_val}
        # Create bitfield packet and add the `DetailedStatus` flag at the start:
        packet_bytes = b'\xD5' + create_bitfield(test_vals)

        # Print the bits of the compiled packet:
        if print_test_packet_bits:
            print(bytes_to_binstr(packet_bytes))

        # Decode the packet:
        packet = parse_packet(packet_bytes)
        packet = cast(WatchdogDetailedStatusPacket, packet)

        # # Ensure only the designated field can have a non-zero value and that its value is correct:
        for checking_field_name in packet.BITFIELD_ALLOCATIONS.keys():
            checking_field_val = getattr(
                packet.custom_payload, checking_field_name)
            err_msg = f"testing {checking_field_name=}, {checking_field_val=} <-> {test_field_name=}, {test_val=}"
            if checking_field_name == test_field_name:
                assert checking_field_val == test_val, err_msg
            else:
                assert checking_field_val == 0, err_msg

    # Advance the bit head:
    idx_bit_head += test_field_bits
