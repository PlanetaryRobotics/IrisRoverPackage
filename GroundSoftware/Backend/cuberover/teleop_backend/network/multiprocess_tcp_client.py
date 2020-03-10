import multiprocessing
import select
import socket

from teleop_backend.utils import signal_utils
import warnings

class MultiprocessTcpClient:
    RECV_MAX_SIZE = 4096
    MAX_SELECT_TIMEOUT = 10

    def __init__(self,
                 application_wide_shutdown_event: multiprocessing.Event,
                 select_timeout: float=0.5):
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
      #patch multiprocessing
    def connect(self, address: str, port: int):
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
        self.__data_handlers.append(handler)

    def send(self, data: bytes, flags=0): # assert_called_once_with
        print("[[MultiprocessTcpClient]: Attempting to send {} bytes of data".format(len(data)))
        result = self.__sock.sendall(data, flags)
        if result is None:
            print("[MultiprocessTcpClient]: Send successful")
        else:
            print("[MultiprocessTcpClient]: Send unsuccessful, result={}".format(result))

    def shutdown(self):
        self.__this_client_shutdown_event.set()

        # We give one second after the select will time out for the recv thread to
        try:
            self.__recv_proc.join(timeout=self.__select_timeout + 1)
        except multiprocessing.TimeoutError:
            self.__recv_proc.terminate()

    @staticmethod
    def recv_data_from_socket(socket, data_handlers):
        chunk = socket.recv(MultiprocessTcpClient.RECV_MAX_SIZE)
        if len(chunk) == 0:
            print("[MultiprocessTcpClient]: Peer closed the connection")
            return False
        else:
            print("[MultiprocessTcpClient]: Got {} bytes of data".format(len(chunk)))
        if(len(data_handlers) == 0):
            warnings.warn("Empty data_handlers", Warning)
        for d in data_handlers:
            d.new_bytes(chunk)
        return True
        
    @staticmethod
    def recv_task(app_wide_shutdown_event,
                  this_client_shutdown_event,
                  sock,
                  select_timeout,
                  data_handlers):
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
