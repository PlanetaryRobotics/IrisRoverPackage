import enum
import multiprocessing
import struct
import sys

from fprime_gds.common.utils import data_desc_type


HEADER_A5A5_SIZE_IN_BYTES = len(b"A5A5 ")
HEADER_DESTINATION_SIZE_IN_BYTES = len(b"FSW ")
SIZE_OR_CMD_DESCRIPTOR_SIZE_IN_BYTES = 4
CMD_SIZE_SIZE_IN_BYTES = 4
TYPE_DESCRIPTOR_SIZE_IN_BYTES = 4


class MessageParsingStateMachine:
    """A state machine that parses serialized F Prime messages from a stream of serialized byte data.

    Serialized F Prime messages have the following formats:

    Serialized command format:
        +--------------------------------+
        | Header = "A5A5 "               |
        | (5 byte string)                |
        +--------------------------------+
        | Destination = "GUI " or "FSW " |
        | (4 byte string)                |
        +--------------------------------+
        | Command descriptor             |
        | (0x5A5A5A5A)                   |
        | (4 byte number)                |
        +--------------------------------+
        | Length of descriptor, opcode,  |
        | and argument data              |
        | (4 bytes)                      |
        +--------------------------------+
        | Descriptor type = 0            |
        | (4 bytes)                      |
        +--------------------------------+
        | Op code                        |
        | (4 bytes)                      |
        +--------------------------------+
        | Argument 1 value               |
        +--------------------------------+
        | Argument 2 value               |
        +--------------------------------+
        | ...                            |
        +--------------------------------+
        | Argument n value               |
        +--------------------------------+

    Serialized event format:
        +--------------------------------+
        | Header = "A5A5 "               |
        | (5 byte string)                |
        +--------------------------------+
        | Destination = "GUI " or "FSW " |
        | (4 byte string)                |
        +--------------------------------+
        | Length of descriptor, ID,      |
        | and argument data              |
        | (4 bytes)                      |
        +--------------------------------+
        | Descriptor type = 2            |
        | (4 bytes)                      |
        +--------------------------------+
        | ID                             |
        | (4 bytes)                      |
        +--------------------------------+
        | Argument 1 value               |
        +--------------------------------+
        | Argument 2 value               |
        +--------------------------------+
        | ...                            |
        +--------------------------------+
        | Argument n value               |
        +--------------------------------+

    Serialized telemetry format:
        +--------------------------------+
        | Header = "A5A5 "               |
        | (5 byte string)                |
        +--------------------------------+
        | Destination = "GUI " or "FSW " |
        | (4 byte string)                |
        +--------------------------------+
        | Length of descriptor, ID,      |
        | and value data                 |
        | (4 bytes)                      |
        +--------------------------------+
        | Descriptor type = 1            |
        | (4 bytes)                      |
        +--------------------------------+
        | ID                             |
        | (4 bytes)                      |
        +--------------------------------+
        | Value                          |
        | (variable bytes, based on type)|
        +--------------------------------+

    Put side-by-side and described in a more generic way, we get the following:

        Serialized command format:          Serialized telemetry format:         Serialized event format:
        +--------------------------------+  +--------------------------------+  +--------------------------------+
        | Header = "A5A5 "               |  | Header = "A5A5 "               |  | Header = "A5A5 "               |
        | (5 byte string)                |  | (5 byte string)                |  | (5 byte string)                |
        +--------------------------------+  +--------------------------------+  +--------------------------------+
        | Destination = "GUI " or "FSW " |  | Destination = "GUI " or "FSW " |  | Destination = "GUI " or "FSW " |
        | (4 byte string)                |  | (4 byte string)                |  | (4 byte string)                |
        +--------------------------------+  +--------------------------------+  +--------------------------------+
        | Command descriptor             |
        | (0x5A5A5A5A)                   |          Nothing (0 bytes)                  Nothing (0 bytes)
        | (4 byte number)                |
        +--------------------------------+  +--------------------------------+  +--------------------------------+
        | Length of descriptor, opcode,  |  | Length of descriptor, ID,      |  | Length of descriptor, ID,      |
        | and argument data              |  | and argument data              |  | and argument data              |
        | (4 bytes)                      |  | (4 bytes)                      |  | (4 bytes)                      |
        +--------------------------------+  +--------------------------------+  +--------------------------------+
        | Descriptor type = 0            |  | Descriptor type = 1            |  | Descriptor type = 2            |
        | (4 bytes)                      |  | (4 bytes)                      |  | (4 bytes)                      |
        +--------------------------------+  +--------------------------------+  +--------------------------------+
        | Data                           |  | Data                           |  | Data                           |
        | (n bytes)                      |  | (n bytes)                      |  | (n bytes)                      |
        +--------------------------------+  +--------------------------------+  +--------------------------------+

    By looking at the messages this way, it becomes much easier to explain this state machine. All three message types
    share the header and destination fields, so the first thing that is done is to look for those bytes. Waiting for the
    header "A5A5 " bytes to arrive the first state, WAITING_FOR_HEADER_A5A5. Next we wait for the destination string,
    either "GUI " or "FSW ", in our second state, WAITING_FOR_HEADER_DESTINATION.

    What happens next is pretty much the only message-type-specific behavior we have. For commands, the next thing we
    expect is a 4 byte number, 0x5A5A5A5A, while for telemetry and event we expect the length of the descriptor type and
    data (i.e. the length of the remaining bytes in the message). Both of these are 4 byte numbers, so we wait for the
    next 4 bytes in the third state, WAITING_FOR_SIZE_OR_CMD_DESCRIPTOR. If we find that these 4 bytes were the number
    0x5A5A5A5A, then we know that our current message is a command so we know that we still need to read the length of
    the descriptor type and data, which happens in the fourth statue, WAITING_FOR_CMD_SIZE. After reading the command
    size, we then move into the fifth stage, WAITING_FOR_TYPE_DESCRIPTOR. On the other hand, if the 4 byte number that
    we received in WAITING_FOR_SIZE_OR_CMD_DESCRIPTOR was NOT 0x5A5A5A5A, then we take it to be the length of the
    descriptor type and data of a telemetry or event message. In this case, we move directly from
    WAITING_FOR_SIZE_OR_CMD_DESCRIPTOR to the fifth stage, WAITING_FOR_TYPE_DESCRIPTOR.

    At this point, all 3 message types will be in WAITING_FOR_TYPE_DESCRIPTOR, and the behavior from this point on is
    the same for all message types. In WAITING_FOR_TYPE_DESCRIPTOR we wait for the next 4 byte integer. This integer
    indicates whether the message is a command, event, or telemetry. We record the message type then move to the sixth
    and final stage, WAITING_FOR_DATA. The number of bytes of data is the length of the type descriptor and data (this
    was read in WAITING_FOR_CMD_SIZE for command messages or in WAITING_FOR_SIZE_OR_CMD_DESCRIPTOR for event or
    telemetry messages) minus 4 bytes (the length of the type descriptor). In the WAITING_FOR_DATA state, we wait until
    we have accumulated all of the data bytes, then we send the message data out to all consumers by putting a copy of
    the data on all registered completed message queues.

    This state machine is fed by passing bytes of data to the new_bytes() function. All data bytes passed to this
    function are appended to a byte array of unknown data. As we move through the stages and accumulate message data,
    the data is moved out of the unknown data byte array and into one of two places: all bytes in the message except for
    the data bytes (the bytes consumed by the stage WAITING_FOR_DATA) are accumulated into a message header byte array.
    The data bytes are accumulated into a separate message contents byte array. This is done because the message
    decoders included in the F Prime Python libraries expect to receive ONLY the message contents bytes. What is put
    onto the completed message queue is actually a tuple of three values: the message type, the message header bytes,
    and the message contents bytes.

    Attributes:
        __state: The current state of this state machine.
        __unknown_data_buffer: The unknown data byte array into which all data received via new_bytes() is appended. As
                               a message is parsed, data is moved out of this byte array and into the message header or
                               message contents byte arrays.
        __unknown_data_next_unchecked_start_index: The start index in the unknown data byte array at which we will begin
                                                   looking for the header "A5A5 " string. If we've received incomplete
                                                   message data that starting in the middle of the message, it is
                                                   possible that we will go through all of the bytes looking for "A5A5 "
                                                   and never finding it. If we then receive a new message, it would be
                                                   inefficient to search from the beginning of our unknown data buffer
                                                   again, since we've already searched through part of it and know that
                                                   there is no "A5A5 " string in it. This variable is used to store the
                                                   index at which the previous failed search ended, so that the next
                                                   search does not need to repeat the work of the previous search.
        __msg_header_buffer: The message header byte array into which message header data is accumulated as a message is
                             parsed. In this case "message header data" consists of all message data other then the data
                             (i.e. the message bytes following the type descriptor field).
        __msg_contents_buffer: The message contents byte array into which message contents data is accumulated as a
                               message is parsed. In this case "message contents data" refers to all data following the
                               type descriptor field of each message type.
        __msg_type: The message type of the message currently being parsed.
        __data_bytes_remaining: The number of data (AKA message contents data) bytes that are needed to complete the
                                message currently being parsed.
        __completed_message_callback_queues: A list of queues onto which completed messages will be placed.
                                             Specifically, a tuple of message type, message header bytes, and message
                                             contents bytes.
        __destination_is_fsw: Whether or not the destination of the message currently being parsed is FSW. This is only
                              used to print a warning if we find any events or telemetry with FSW as their destination,
                              since that is not an expected use case.
        __state_processing_functions: A dictionary mapping the possible states of this state machine to the methods
                                      that execute the behavior of that state. Used to simplify dispatch to a particular
                                      state method.
    """

    @enum.unique
    class State(enum.Enum):
        """The states of this state machine. See the class description for descriptions of the states."""
        WAITING_FOR_HEADER_A5A5 = 1
        WAITING_FOR_HEADER_DESTINATION = 2
        WAITING_FOR_SIZE_OR_CMD_DESCRIPTOR = 3
        WAITING_FOR_CMD_SIZE = 4
        WAITING_FOR_TYPE_DESCRIPTOR = 5
        WAITING_FOR_DATA = 6

    def __init__(self):
        """Constructor."""
        self.__state = MessageParsingStateMachine.State.WAITING_FOR_HEADER_A5A5
        self.__unknown_data_buffer = bytearray()
        self.__unknown_data_next_unchecked_start_index = 0
        self.__msg_header_buffer = bytearray()
        self.__msg_contents_buffer = bytearray()
        self.__msg_type = None
        self.__data_bytes_remaining = 0
        self.__completed_message_callback_queues = []
        self.__destination_is_fsw = False
        self.__state_processing_functions = {
            MessageParsingStateMachine.State.WAITING_FOR_HEADER_A5A5:
                self.__process_state_waiting_for_header_a5a5,
            MessageParsingStateMachine.State.WAITING_FOR_HEADER_DESTINATION:
                self.__process_state_waiting_for_header_destination,
            MessageParsingStateMachine.State.WAITING_FOR_SIZE_OR_CMD_DESCRIPTOR:
                self.__process_state_waiting_for_size_or_cmd_descriptor,
            MessageParsingStateMachine.State.WAITING_FOR_CMD_SIZE:
                self.__process_state_waiting_for_cmd_size,
            MessageParsingStateMachine.State.WAITING_FOR_TYPE_DESCRIPTOR:
                self.__process_state_waiting_for_type_descriptor,
            MessageParsingStateMachine.State.WAITING_FOR_DATA:
                self.__process_state_waiting_for_data,
        }

    def register_completed_message_queue(self, completed_message_queue: multiprocessing.Queue):
        """Registers the given queue to the list that will receive completed messages when they are parsed.

        Args:
            completed_message_queue: The queue to be registered.

        Returns:
            None
        """
        self.__completed_message_callback_queues.append(completed_message_queue)

    def new_bytes(self, byte_data: bytes):
        """Appends the given bytes to the unknown data byte array, then attempts to parse messages out of it.

        Args:
            byte_data: Serialized but possibly unframed and possibly incomplete F Prime message data.

        Returns:
            None
        """
        if len(byte_data) <= 0:
            return

        # Add our byte_data to the unknown data buffer. We'll copy anything that we determine to be part of a
        # message into the message buffer later.
        #print("Got new data:", byte_data)
        self.__unknown_data_buffer.extend(byte_data)

        # We want to keep traversing the state machine until one of the states indicates that no further processing can
        # be done on the data in the unknown data byte array (which it indicates by returning False)
        keep_spinning = True
        while keep_spinning:
            state_processing_function = self.__state_processing_functions.get(self.__state, None)
            if state_processing_function:
                keep_spinning = state_processing_function()
            else:
                raise RuntimeError("Invalid state: {}".format(self.__state))

    def __process_state_waiting_for_header_a5a5(self):
        """Performs the actions of the WAITING_FOR_HEADER_A5A5 state.

        If the unknown data buffer has at least 5 bytes, then we step through it, checking each 5 byte substring of the
        full byte array to see if it is "A5A5 ". If one is then we start processing a new message at that point in the
        unknown data byte array and transition to WAITING_FOR_HEADER_DESTINATION. Otherwise, we remain in this state.

        Returns:
            True if the state machine should keep spinning, or False if we cannot progress further with the current
            unknown data byte array.
        """
        if len(self.__unknown_data_buffer) >= HEADER_A5A5_SIZE_IN_BYTES:
            # We have enough bytes for a header, so step a header-sized window through our buffer byte-by-byte,
            # looking for a valid header

            possible_starts = (len(self.__unknown_data_buffer) - HEADER_A5A5_SIZE_IN_BYTES) + 1
            for i in range(self.__unknown_data_next_unchecked_start_index, possible_starts):
                potential_a5a5_bytes = self.__unknown_data_buffer[i:i + HEADER_A5A5_SIZE_IN_BYTES]

                if potential_a5a5_bytes == b"A5A5 ":
                    # We found our header byte string, so we assume that this is the start of a new message.
                    # Copy the header byte string and advance the unknown data buffer past it, then update our state
                    # and recurse
                    self.__msg_header_buffer.clear()
                    self.__msg_contents_buffer.clear()
                    self.__msg_header_buffer.extend(potential_a5a5_bytes)
                    self.__unknown_data_buffer = self.__unknown_data_buffer[i + HEADER_A5A5_SIZE_IN_BYTES:]
                    self.__state = MessageParsingStateMachine.State.WAITING_FOR_HEADER_DESTINATION
                    self.__unknown_data_next_unchecked_start_index = 0
                    # self.new_bytes(b"")
                    return True

            # We search all of the possible header start points and didn't find a header, so we don't need to
            # re-search any of them the next time we get data. To avoid this, record the next start point that we
            # should search.
            self.__unknown_data_next_unchecked_start_index = possible_starts

        return False

    def __process_state_waiting_for_header_destination(self):
        """Performs the actions of the WAITING_FOR_HEADER_DESTINATION state.

        If the unknown data buffer has at least 4 bytes, check if the first 4 bytes are "FSW " or "GUI ". If not, we
        transition back to WAITING_FOR_HEADER_A5A5. Otherwise, store the destination and transition to
        WAITING_FOR_SIZE_OR_CMD_DESCRIPTOR.

        Returns:
            True if the state machine should keep spinning, or False if we cannot progress further with the current
            unknown data byte array.
        """
        if len(self.__unknown_data_buffer) >= HEADER_DESTINATION_SIZE_IN_BYTES:
            # We have enough bytes in the unknown data buffer for our destination string. Make sure it is one of the
            # valid destinations, and if not reset
            destination_bytes = self.__unknown_data_buffer[:HEADER_DESTINATION_SIZE_IN_BYTES]
            if destination_bytes == b"FSW " or destination_bytes == b"GUI ":
                # It was a valid destination string, so simply copy it over to the message buffer, advance the
                # unknown data buffer, update our state, and recurse. Also record if it is GUI or not, because we
                # expect that only commands will go to FSW.
                self.__destination_is_fsw = (destination_bytes == b"FSW ")

                self.__msg_header_buffer.extend(destination_bytes)
                self.__unknown_data_buffer = self.__unknown_data_buffer[HEADER_DESTINATION_SIZE_IN_BYTES:]
                self.__state = MessageParsingStateMachine.State.WAITING_FOR_SIZE_OR_CMD_DESCRIPTOR
                # self.new_bytes(b"")
                return True
            else:
                # It was an invalid destination string, so reset. Also print a warning because this shouldn't happen
                print("Expected message destination bytes of {} or {},but got "
                      "{}".format(b"FSW ", b"GUI ", destination_bytes), file=sys.stderr)
                self.__msg_header_buffer.clear()
                self.__state = MessageParsingStateMachine.State.WAITING_FOR_HEADER_A5A5

        return False

    def __process_state_waiting_for_size_or_cmd_descriptor(self):
        """Performs the actions of the WAITING_FOR_SIZE_OR_CMD_DESCRIPTOR state.

        If the unknown data buffer has at least 4 bytes, parse the first 4 bytes as a big-endian integer. If it is
        0x5A5A5A5A, then transition to WAITING_FOR_CMD_SIZE, otherwise store the integer as the data size and
        transition to WAITING_FOR_TYPE_DESCRIPTOR.

        Returns:
            True if the state machine should keep spinning, or False if we cannot progress further with the current
            unknown data byte array.
        """
        if len(self.__unknown_data_buffer) >= SIZE_OR_CMD_DESCRIPTOR_SIZE_IN_BYTES:
            # We have enough bytes in the unknown data buffer for either the size or command descriptor, both of
            # which are unsigned 4-byte integers that were serialized as big-endian.
            size_or_cmd_descriptor_bytes = self.__unknown_data_buffer[:SIZE_OR_CMD_DESCRIPTOR_SIZE_IN_BYTES]
            size_or_cmd_descriptor = struct.unpack(">I", size_or_cmd_descriptor_bytes)[0]

            # Figure out if this is a command descriptor or a size
            if size_or_cmd_descriptor == 0x5A5A5A5A:
                # It's a command descriptor so we want to read the command size next
                self.__state = MessageParsingStateMachine.State.WAITING_FOR_CMD_SIZE
            else:
                # It was not the command descriptor, so we assume it is the size of a event of telemetry message.
                # We expect that this is going to GUI, if not we print a warning. However, we'll keep working
                # despite the warning.
                if self.__destination_is_fsw:
                    print("Decoded a telemetry or event message with destination set as FSW!", file=sys.stderr)

                # Store the expected data length for when we read the data. However, next we'll read the message
                # type descriptor
                self.__data_bytes_remaining = size_or_cmd_descriptor
                self.__state = MessageParsingStateMachine.State.WAITING_FOR_TYPE_DESCRIPTOR

            # In both cases, copy the data to the message buffer, advance the unknown data buffer, and recurse.
            self.__msg_header_buffer.extend(size_or_cmd_descriptor_bytes)
            self.__unknown_data_buffer = self.__unknown_data_buffer[SIZE_OR_CMD_DESCRIPTOR_SIZE_IN_BYTES:]
            # self.new_bytes(b"")
            return True

        return False

    def __process_state_waiting_for_cmd_size(self):
        """Performs the actions of the WAITING_FOR_CMD_SIZE state.

        If the unknown data buffer has at least 4 bytes, parse the first 4 bytes as a big-endian integer. Store the
        integer as the data size then transition to WAITING_FOR_TYPE_DESCRIPTOR.

        Returns:
            True if the state machine should keep spinning, or False if we cannot progress further with the current
            unknown data byte array.
        """
        if len(self.__unknown_data_buffer) >= CMD_SIZE_SIZE_IN_BYTES:
            # We have enough bytes in the unknown data buffer for the command size, which is an unsigned 4-byte
            # integer that was serialized as big-endian.
            cmd_size_bytes = self.__unknown_data_buffer[:CMD_SIZE_SIZE_IN_BYTES]
            cmd_size = struct.unpack(">I", cmd_size_bytes)[0]

            # Our next step is reading the type descriptor, but we store the expected data length for when we read
            # the data
            self.__data_bytes_remaining = cmd_size
            self.__state = MessageParsingStateMachine.State.WAITING_FOR_TYPE_DESCRIPTOR

            # As per usual, copy the data to the message buffer, advance the unknown data buffer, and recurse.
            self.__msg_header_buffer.extend(cmd_size_bytes)
            self.__unknown_data_buffer = self.__unknown_data_buffer[CMD_SIZE_SIZE_IN_BYTES:]
            # self.new_bytes(b"")
            return True

        return False

    def __process_state_waiting_for_type_descriptor(self):
        """Performs the actions of the WAITING_FOR_TYPE_DESCRIPTOR state.

        If the unknown data buffer has at least 4 bytes, parse the first 4 bytes as a big-endian integer. If the integer
        is not one of the three expected message types, then restart by transitioning back to WAITING_FOR_HEADER_A5A5.
        Otherwise, store the message type, subtract 4 from the data size, and transition to WAITING_FOR_DATA.

        Returns:
            True if the state machine should keep spinning, or False if we cannot progress further with the current
            unknown data byte array.
        """
        if len(self.__unknown_data_buffer) >= TYPE_DESCRIPTOR_SIZE_IN_BYTES:
            # We have enough bytes in the unknown data buffer for the type descriptor, which is an unsigned 4-byte
            # integer that was serialized as big-endian.
            type_descriptor_bytes = self.__unknown_data_buffer[:TYPE_DESCRIPTOR_SIZE_IN_BYTES]
            type_descriptor = struct.unpack(">I", type_descriptor_bytes)[0]

            if (type_descriptor == data_desc_type.DataDescType["FW_PACKET_COMMAND"].value or
                    type_descriptor == data_desc_type.DataDescType["FW_PACKET_TELEM"].value or
                    type_descriptor == data_desc_type.DataDescType["FW_PACKET_LOG"].value):
                self.__msg_type = data_desc_type.DataDescType(type_descriptor)

                # The type descriptor is counted as part of the data, so decrease the remaining data byte count
                # accordingly
                self.__data_bytes_remaining -= TYPE_DESCRIPTOR_SIZE_IN_BYTES
                self.__state = MessageParsingStateMachine.State.WAITING_FOR_DATA

                # As per usual, copy the data to the message buffer, advance the unknown data buffer, and recurse.
                self.__msg_header_buffer.extend(type_descriptor_bytes)
                self.__unknown_data_buffer = self.__unknown_data_buffer[TYPE_DESCRIPTOR_SIZE_IN_BYTES:]
                # self.new_bytes(b"")
                return True
            else:
                print("Got unexpected message type descriptor: {}".format(type_descriptor), file=sys.stderr)
                self.__msg_header_buffer.clear()
                self.__state = MessageParsingStateMachine.State.WAITING_FOR_HEADER_A5A5

        return False

    def __process_state_waiting_for_data(self):
        """Performs the actions of the WAITING_FOR_DATA state.

        Copy as many bytes as we can towards completing the message. If there are enough bytes to complete the message,
        send it out by calling __handle_completed_message(). That function also resets several of the instance variables
        and transitions the state back to WAITING_FOR_HEADER_A5A5 in order to be ready to parse the next message. If
        there is any remaining unknown data after completing the message then we continue to spin in the state machine,
        otherwise the state machine stops processing until new data is received.

        Returns:
            True if the state machine should keep spinning, or False if we cannot progress further with the current
            unknown data byte array.
        """
        # Copy only up to the expected number of bytes into the message buffer.
        num_unknown_bytes = len(self.__unknown_data_buffer)
        num_data_bytes_for_msg = min(self.__data_bytes_remaining, num_unknown_bytes)
        num_remaining_bytes = num_unknown_bytes - num_data_bytes_for_msg
        self.__msg_contents_buffer += self.__unknown_data_buffer[:num_data_bytes_for_msg]
        self.__data_bytes_remaining -= num_data_bytes_for_msg

        # Advance the unknown data buffer
        self.__unknown_data_buffer = self.__unknown_data_buffer[num_data_bytes_for_msg:]

        # If we were able to complete a message with these bytes, then we want to forward the completed message and
        # then handle any remaining bytes.
        if self.__data_bytes_remaining == 0:
            self.__handle_completed_message()

            # If there's still data in the unknown data buffer, then recurse so we can start processing it as a new
            # message.
            if num_remaining_bytes > 0:
                # self.new_bytes(b"")
                return True

        return False

    def __handle_completed_message(self):
        """Sends a completed message out on all registered completed message queues, then resets the state.

        Resetting the state involves clearing our message byte arrays and resetting the state to
        WAITING_FOR_HEADER_A5A5.

        Returns:
            None
        """
        for queue in self.__completed_message_callback_queues:
            # Casting the byte arrays to bytes is needed because the bytearrays lose their contents when they go from
            # one process to another. The bytes objects keep their data.
            queue.put((self.__msg_type, bytes(self.__msg_header_buffer), bytes(self.__msg_contents_buffer)))

        if len(self.__completed_message_callback_queues) == 0:
            print("Warning: Parsed a complete message, but no message callback queue is currently registered",
                  file=sys.stderr)

        # Clear the message buffer now just so we're sure we don't have anything left-over of the last message.
        self.__msg_header_buffer.clear()
        self.__msg_contents_buffer.clear()

        # Reset the state to parse another message starting with the header
        self.__unknown_data_next_unchecked_start_index = 0
        self.__state = MessageParsingStateMachine.State.WAITING_FOR_HEADER_A5A5
