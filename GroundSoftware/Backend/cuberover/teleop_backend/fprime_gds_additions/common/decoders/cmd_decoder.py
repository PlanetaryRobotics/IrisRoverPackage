"""
@brief Decoder for command data

This decoder takes in serialized commands, parses them, and packages the results
in cmd_data objects.

Example data structure:
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
"""

from __future__ import print_function
import copy

from fprime_gds.common.decoders import decoder
from fprime_gds.common.data_types import cmd_data
from fprime.common.models.serialize import u32_type
from fprime.common.models.serialize.type_exceptions import *
import traceback


class CmdDecoder(decoder.Decoder):
    """Decoder class for command data"""

    def __init__(self, cmd_dict):
        """
        EventDecoder class constructor

        Args:
            cmd_dict: Command dictionary. Event IDs should be keys and
                        EventTemplate objects should be values

        Returns:
            An initialized EventDecoder object.
        """
        super(CmdDecoder, self).__init__()

        self.__dict = cmd_dict

    def data_callback(self, data):
        """
        Function called to pass data to the decoder class

        Args:
            data: Binary data to decode and pass to registered consumers
        """
        result = self.decode_api(data)

        # Make sure we don't send None data
        if result is not None:
            self.send_to_all(result)

    def decode_api(self, data):
        """
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        Args:
            data: Binary data to decode

        Returns:
            Parsed version of the event data in the form of a CmdData object
            or None if the data is not decodable
        """
        ptr = 0

        # Decode opcode here...
        opcode_obj = u32_type.U32Type()
        opcode_obj.deserialize(data, ptr)
        ptr += opcode_obj.getSize()
        cmd_opcode = opcode_obj.val

        if cmd_opcode in self.__dict:
            cmd_temp = self.__dict[cmd_opcode]

            arg_vals = self.decode_args(data, ptr, cmd_temp)

            return cmd_data.CmdData([str(arg.val) for arg in arg_vals], cmd_temp)
        else:
            print("Command decode error: id %d not in dictionary" % cmd_opcode)
            return None

    def decode_args(self, arg_data, offset, template):
        """
        Decodes the serialized command arguments

        The command arguments are each serialized and then appended to each other.
        Parse that section of the data into the individual arguments.

        Args:
            arg_data: Serialized argument data to parse
            offset: Offset into the arg_data where parsing should start
            template: CmdTemplate object that describes the type of command the
                      arg_data goes to.

        Returns:
            Parsed arguments in a tuple (order the same as they were parsed in).
            Each element in the tuple is an instance of the same class as the
            corresponding arg_type object in the template parameter. Returns
            none if the arguments can't be parsed
        """
        arg_results = []
        args = template.get_args()

        # For each argument, use the arg_obj deserialize method to get the value
        for arg in args:
            (arg_name, arg_desc, template_arg_obj) = arg

            # Create a new instance of the argument's type object so we don't
            # use the template's object for deserialization and storage of the
            # parsed argument value.
            arg_obj = copy.deepcopy(template_arg_obj)

            try:
                arg_obj.deserialize(arg_data, offset)
                arg_results.append(arg_obj)
            except TypeException as e:
                print("Command decode exception %s" % (e.getMsg()))
                traceback.print_exc()
                return None

            offset = offset + arg_obj.getSize()

        return tuple(arg_results)


if __name__ == "__main__":
    pass
