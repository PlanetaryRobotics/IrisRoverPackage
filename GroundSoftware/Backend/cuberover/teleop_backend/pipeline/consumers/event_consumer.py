import multiprocessing

from fprime_gds.common.data_types import event_data

from teleop_backend.pipeline.consumers import base_consumer
from teleop_backend.msg_format import format_conversion


class EventConsumer(base_consumer.BaseConsumer):
    """A consumer that consumes the output of an event decoder."""

    def __init__(self, queue: multiprocessing.Queue):
        """Constructor.

        Args:
            queue: The output queue for converted dictionaries.
        """
        super().__init__(queue)

    def _convert_data_obj_to_database(self, data_obj: event_data.EventData):
        """Converts the given F Prime event data object into a database-format event dictionary.

        Args:
            data_obj: An F Prime event data object to be converted.

        Returns:
            A database-format event dictionary.
        """
        return format_conversion.convert_event_from_data_type_to_database(data_obj)
