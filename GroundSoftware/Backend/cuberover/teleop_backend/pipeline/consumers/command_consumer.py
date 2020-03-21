import multiprocessing

from fprime_gds.common.data_types import cmd_data

from teleop_backend.pipeline.consumers import base_consumer
from teleop_backend.msg_format import format_conversion


class CommandConsumer(base_consumer.BaseConsumer):
    """A consumer that consumes the output of a command decoder."""

    def __init__(self, queue: multiprocessing.Queue):
        """Constructor.

        Args:
            queue: The output queue for converted dictionaries.
        """
        super().__init__(queue)

    def _convert_data_obj_to_database(self, data_obj: cmd_data.CmdData) -> dict:
        """Converts the given F Prime command data object into a database-format command dictionary.

        Args:
            data_obj: An F Prime command data object to be converted.

        Returns:
            A database-format command dictionary.
        """
        return format_conversion.convert_command_from_data_type_to_database(data_obj)
