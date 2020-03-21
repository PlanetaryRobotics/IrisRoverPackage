import abc
import multiprocessing


class BaseConsumer(metaclass=abc.ABCMeta):
    """The abstract base class for F Prime data object consumers, which consume the output of the F Prime decoders.

    Each concrete subclass must implement the _convert_data_obj_to_database() function such that the received
    F Prime data objects are converted to equivalent database-format dictionaries. Then, when a decoder outputs
    a data object, the appropriate consumer will receive that data object, convert it to database-format, then place
    it onto a queue passed to the constructor.

    This base class implements receiving the data object from the decoders, calling the conversion function, then
    putting the conversion result onto the queue.

    Attributes:
        __queue: The output queue onto which database-format message dictionaries will be placed.
    """


    def __init__(self, queue: multiprocessing.Queue):
        """Constructor.

        Args:
            queue: The output queue.
        """
        self.__queue = queue

    def data_callback(self, data_obj):
        """The callback invoked by the F Prime message decoders when they have decoded a new data object.

        This implementation converts the data object to a database-format message dictionary, then puts that dictionary
        onto the output queue.

        Args:
            data_obj: An F Prime data object output by an F Prime Decoder.

        Returns:
            None
        """
        database_obj = self._convert_data_obj_to_database(data_obj)
        self.__queue.put(database_obj)

    @abc.abstractmethod
    def _convert_data_obj_to_database(self, data_obj) -> dict:
        """Converts the given data object into a database-format message dictionary.

        Must be overridden by concrete subclasses.

        Args:
            data_obj: An F Prime data object to be converted.

        Returns:
            A database-format message dictionary.
        """
        raise NotImplementedError("Concrete subclasses of BaseConsumer must implement this function")
