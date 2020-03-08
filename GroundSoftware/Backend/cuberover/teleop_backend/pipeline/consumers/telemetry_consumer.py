import multiprocessing

from fprime_gds.common.data_types import ch_data

from teleop_backend.pipeline.consumers import base_consumer
from teleop_backend.msg_format import format_conversion


class TelemetryConsumer(base_consumer.BaseConsumer):
    """A consumer that consumes the output of a telemetry decoder."""

    def __init__(self, queue: multiprocessing.Queue):
        """Constructor.

        Args:
            queue: The output queue for converted dictionaries.
        """
        super().__init__(queue)

    def _convert_data_obj_to_database(self, data_obj: ch_data.ChData):
        """Converts the given F Prime telemetry data object into a database-format telemetry dictionary.

        Args:
            data_obj: An F Prime telemetry data object to be converted.

        Returns:
            A database-format telemetry dictionary.
        """
        return format_conversion.convert_telemetry_from_data_type_to_database(data_obj)
