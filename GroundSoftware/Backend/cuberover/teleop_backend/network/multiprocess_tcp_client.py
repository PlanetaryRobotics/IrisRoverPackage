import multiprocessing
import select
import socket

from teleop_backend.utils import signal_utils

class MultiprocessTcpClient:
    """A TCP client for connecting to "Houston" that has its own process for receiving data.

    This class also supports sending data through the same socket that is used for receiving data by the process.

    Attributes:
        RECV_MAX_SIZE: A class variable that indicates the max number of bytes a single recv can accept.
        MAX_SELECT_TIMEOUT: A class variable containing the max allowed timeout for the select call in the receiving
                            task
        __sock: The socket used to send and receive data.
        __recv_proc: The internal receiving process.
        __data_handlers: A list of objects that at a minimum have a function named "new_bytes" that take a bytes object
                         as its only parameter. Each data handler in the list receives a copy of all bytes received by
                         this client.
        __application_wide_shutdown_event: A multiprocessing.Event used to signal the shutdown of all process that are
                                           part of the teleop backend.
        __this_client_shutdown_event: A multiprocessing.Event used to signal the shutdown of only the internal receiving
                                      process.
    """
    RECV_MAX_SIZE = 4096
    MAX_SELECT_TIMEOUT = 10

    def __init__(self,
                 application_wide_shutdown_event: multiprocessing.Event,
                 select_timeout: float = 0.5):
        """Constructor.

        Args:
            application_wide_shutdown_event: The event used to signal the shutdown of all processes in the application.
            select_timeout: The timeout, in seconds, of the select call in the receiving task.
        """
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.__recv_proc = None
        self.__data_handlers = []
        self.__application_wide_shutdown_event = application_wide_shutdown_event
        self.__this_client_shutdown_event = multiprocessing.Event()

        if select_timeout > self.MAX_SELECT_TIMEOUT:
            raise ValueError("The given select timeout ({}) "
                             "is larger than the maximum ({})".format(select_timeout,self.MAX_SELECT_TIMEOUT))

        self.__select_timeout = select_timeout

    def connect(self, address: str, port: int):
        """Connects the internal socket to a server with the given address and port, then starts the receiving process.

        Args:
            address: The server IP address.
            port: The server port.

        Returns:
            None
        """
        self.__sock.connect((address, port))
        self.__recv_proc = multiprocessing.Process(target=MultiprocessTcpClient.recv_task,
                                                   name="MultiprocessTcpClient_recv_task",
                                                   args=(self.__application_wide_shutdown_event,
                                                         self.__this_client_shutdown_event,
                                                         self.__sock,
                                                         self.__select_timeout,
                                                         self.__data_handlers))
        self.__recv_proc.start()

    def register_data_handler(self, handler):
        """Registers an object to be a data handler for this client.

        Each data handler in the list receives a copy of all bytes received by this client.

        Args:
            handler: Any object that has, at a minimum, a method named "new_bytes" that takes a bytes object as its only
                     parameter. This is the method that will be called to give this data handler bytes received by this
                     client.

        Returns:
            None

        """
        self.__data_handlers.append(handler)

    def send(self, data: bytes, flags=0):
        """Synchronously sends all of the given data to the server.

        Args:
            data: The data to be sent to the server.
            flags: Flags to be passed to socket.sendall().

        Returns:
            None

        Raises:
            OSError: If any error occurs.
        """
        print("[MultiprocessTcpClient]: Attempting to send {} bytes of data".format(len(data)))
        result = self.__sock.sendall(data, flags)
        if result is None:
            print("[MultiprocessTcpClient]: Send successful")
        else:
            print("[MultiprocessTcpClient]: Send unsuccessful, result={}".format(result))

    def shutdown(self):
        """Shuts down the receiving process and waits for it to end, terminating it if the wait times out.

        The wait timeout duration that is used is the select timeout given to the constructor plus one second.

        Returns:
            None
        """
        self.__this_client_shutdown_event.set()

        # We give one second after the select will time out for the recv thread to exit
        try:
            self.__recv_proc.join(timeout=self.__select_timeout + 1)
        except multiprocessing.TimeoutError:
            self.__recv_proc.terminate()

    @staticmethod
    def recv_task(app_wide_shutdown_event: multiprocessing.Event,
                  this_client_shutdown_event: multiprocessing.Event,
                  sock: socket.socket,
                  select_timeout: float,
                  data_handlers: list):
        """The receiving task executed by the internal process of this client.

        This task simply receives data and passes it to the data handlers until either the socket is disconnected or one
        of the shutdown flag is set.

        Args:
            app_wide_shutdown_event: An event used to signal a shutdown of all processes in this application
            this_client_shutdown_event: An event used to signal a shutdown of only this process.
            sock: The pre-connected socket from which this task will receive data.
            select_timeout: The timeout duration of the select call in this task
            data_handlers: The list of data_handlers that will receive the bytes received by the socket.

        Returns:
            None
        """
        try:
            signal_utils.setup_signal_handler_for_process(shutdown_event=this_client_shutdown_event)

            while (not app_wide_shutdown_event.is_set() and
                   not this_client_shutdown_event.is_set()):
                ready = select.select([sock], [], [sock], select_timeout)
                if ready[0]:
                    chunk = sock.recv(MultiprocessTcpClient.RECV_MAX_SIZE)
                    if len(chunk) == 0:
                        print("[MultiprocessTcpClient]: Peer closed the connection")
                        break
                    else:
                        print("[MultiprocessTcpClient]: Got {} bytes of data".format(len(chunk)))

                    for d in data_handlers:
                        d.new_bytes(chunk)

                if ready[2]:
                    break
        finally:
            if sock:
                sock.close()
            app_wide_shutdown_event.set()
            print("[MultiprocessTcpClient]: recv task exiting")
