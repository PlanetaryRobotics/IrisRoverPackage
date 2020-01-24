import enum
import multiprocessing
import struct

#

USING_IDL = False
if USING_IDL:
    from teleop_backend.idl_msgs import msg_utilities

    HEADER_SIZE_IN_BYTES = msg_utilities.HEADER_SIZE_IN_BYTES
    GET_MSG_INFO_FROM_HEADER_FUNCTION = msg_utilities.get_msg_info_from_header
else:
    from teleop_backend.commands import command
    import ctypes

    HEADER_SIZE_IN_BYTES = ctypes.sizeof(command.CommandHeaderStruct)
    def get_msg_info_from_header(data, big_endian):
        if not big_endian:
            raise RuntimeError("When not using IDL, data must be transmitted as big endian")

        try:
            header_struct = command.Command.parse_header_from_encoded(data)
            return command.Command, header_struct.data_length
        except AssertionError as err:
            raise ValueError(str(err))

    GET_MSG_INFO_FROM_HEADER_FUNCTION = get_msg_info_from_header


class MessageParsingStateMachine:
    @enum.unique
    class State(enum.Enum):
        WAITING_FOR_HEADER = enum.auto,
        WAITING_FOR_DATA = enum.auto

    def __init__(self, input_bytes_will_be_big_endian: bool):
        self.__state = MessageParsingStateMachine.State.WAITING_FOR_HEADER
        self.__unknown_data_buffer = bytearray()
        self.__unknown_data_next_unchecked_start_index = 0
        self.__msg_buffer = bytearray()
        self.__msg_type = None
        self.__data_bytes_remaining = 0
        self.__completed_message_callback_queues = []
        self.__input_bytes_will_be_big_endian = input_bytes_will_be_big_endian

    def new_bytes(self, byte_data: bytes):
        if self.__state == MessageParsingStateMachine.State.WAITING_FOR_HEADER:
            # Add our byte_data to the unknown data buffer. We'll copy anything that we determine to be part of a
            # message into the message buffer later.
            self.__unknown_data_buffer.extend(byte_data)

            if len(self.__unknown_data_buffer) >= HEADER_SIZE_IN_BYTES:
                # We have enough bytes for a header, so step a header-sized window through our buffer byte-by-byte,
                # looking for a valid header

                possible_starts = (len(self.__unknown_data_buffer) - HEADER_SIZE_IN_BYTES) + 1
                for i in range(self.__unknown_data_next_unchecked_start_index, possible_starts):
                    try:
                        msg_type, data_len = GET_MSG_INFO_FROM_HEADER_FUNCTION(self.__unknown_data_buffer[i:],
                                                                               self.__input_bytes_will_be_big_endian)

                        # Since the function above didn't raise an error, we know that we have a valid header starting
                        # at the current i index. I move forward by copying the header data to the msg buffer, setting
                        # the message types and data bytes remaining, then transitioning the state and recursing with
                        # the remaining data in the unknown data buffer. This results in the appropriate amount of the
                        # remaining data being copied.
                        self.__msg_buffer.clear()

                        self.__msg_buffer.extend(self.__unknown_data_buffer[i:i + HEADER_SIZE_IN_BYTES])
                        self.__msg_type = msg_type
                        self.__data_bytes_remaining = data_len
                        self.__state = MessageParsingStateMachine.State.WAITING_FOR_DATA

                        remaining_bytes = bytes(self.__unknown_data_buffer[i + HEADER_SIZE_IN_BYTES:])
                        if len(remaining_bytes) > 0:
                            self.new_bytes(remaining_bytes)

                        return

                    except ValueError:
                        pass

                # We search all of the possible header start points and didn't find a header, so we don't need to
                # re-search any of them the next time we get data. To avoid this, record the next start point that we
                # should search.
                self.__unknown_data_next_unchecked_start_index = possible_starts
        elif self.__state == MessageParsingStateMachine.State.WAITING_FOR_DATA:
            # Copy only up to the expected number of bytes into the message buffer.
            num_data_bytes_for_msg = min(self.__data_bytes_remaining, len(byte_data))
            num_remaining_bytes = len(byte_data) - num_data_bytes_for_msg
            self.__msg_buffer += byte_data[0:num_data_bytes_for_msg]
            self.__data_bytes_remaining -= num_data_bytes_for_msg

            # If we were able to complete a message with these bytes, then we want to forward the completed message and
            # then handle any remaining bytes.
            if self.__data_bytes_remaining == 0:
                if USING_IDL:
                    msg = self.__msg_type.parse_from_serialized_bytes(bytes(self.__msg_buffer),
                                                                      self.__input_bytes_will_be_big_endian)
                else:
                    msg = command.Command.from_encoded(bytes(self.__msg_buffer))

                for queue in self.__completed_message_callback_queues:
                    queue.put(msg)

                if len(self.__completed_message_callback_queues) == 0:
                    print("Warning: Parsed a complete message, but no message callback queue is currently registered")

                # Clear the message buffer now just so we're sure we don't have anything left-over of the last message.
                self.__msg_buffer.clear()

                # Reset the state to parse another message starting with the header
                self.__unknown_data_buffer.clear()
                self.__unknown_data_next_unchecked_start_index = 0
                self.__state = MessageParsingStateMachine.State.WAITING_FOR_HEADER

                # Lastly, handle any remaining bytes like a new message.
                if num_remaining_bytes > 0:
                    self.new_bytes(byte_data[num_data_bytes_for_msg:])

        else:
            raise RuntimeError("Invalid state: {}".format(self.__state))

    def register_completed_message_queue(self, completed_message_queue: multiprocessing.Queue):
        self.__completed_message_callback_queues.append(completed_message_queue)
