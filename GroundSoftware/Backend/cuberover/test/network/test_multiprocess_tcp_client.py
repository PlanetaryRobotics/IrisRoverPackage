import unittest
from teleop_backend.network import multiprocess_tcp_client
from unittest import mock
import warnings
from teleop_backend.network.multiprocess_tcp_client import EmptyDataHandlerWarning

class TestMultiprocessTcpClient(unittest.TestCase):
    def test_emptyDataHandler(self):
        """
        Checks for when there are no data handlers and there is data
        
        Expected Behavior: Warns EmptyDataHandlerWarning
        """
        data_handlers = []
        with unittest.mock.patch("socket.socket") as mock_socket:
            mock_shutdown_event = unittest.mock.MagicMock()
            tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(mock_shutdown_event)
            mock_socket.recv.return_value = b"my_test_data"
        self.assertWarns(EmptyDataHandlerWarning, tcp_client.recv_data_from_socket, mock_socket, data_handlers)

    @unittest.expectedFailure
    def test_nonEmptyDataHandler(self):
        """
        Checks for when there are data handlers and there is data
        
        Expected Behavior: Does not warn EmptyDataHandlerWarning
        """
        mock_parser = mock.MagicMock()
        data_handlers = [mock_parser]

        with unittest.mock.patch("socket.socket") as mock_socket:
            mock_shutdown_event = unittest.mock.MagicMock()
            tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(mock_shutdown_event)
            mock_socket.recv.return_value = b"my_test_data"
        self.assertWarns(EmptyDataHandlerWarning, tcp_client.recv_data_from_socket, mock_socket, data_handlers)

    def test_noData(self):
        """
        Checks for when no data from socket
        
        Expected Behavior: returns False
        """
        mock_parser = mock.MagicMock()
        data_handlers = [mock_parser]

        with unittest.mock.patch("socket.socket") as mock_socket:
            mock_shutdown_event = unittest.mock.MagicMock()
            tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(mock_shutdown_event)
            mock_socket.recv.return_value = b""
        self.assertFalse(tcp_client.recv_data_from_socket(mock_socket, data_handlers))
        mock_parser.new_bytes.assert_not_called()

    def test_existsData(self):
        """
        Checks for when data exists from socket
        
        Expected Behavior: returns True
        """
        mock_parser = mock.MagicMock()
        data_handlers = [mock_parser]
        
        with unittest.mock.patch("socket.socket") as mock_socket:
            mock_shutdown_event = unittest.mock.MagicMock()
            tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(mock_shutdown_event)
            mock_socket.recv.return_value = b"sample data"
        self.assertTrue(tcp_client.recv_data_from_socket(mock_socket, data_handlers))
        mock_parser.new_bytes.assert_called_once_with(b"sample data")

    def test_IOError(self):
        """
        Checks for when data exists from socket
        
        Expected Behavior: raises IOError
        """
        mock_parser = mock.MagicMock()
        data_handlers = [mock_parser]
        
        with unittest.mock.patch("socket.socket") as mock_socket:
            mock_shutdown_event = unittest.mock.MagicMock()
            tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(mock_shutdown_event)
            mock_socket.recv.side_effect = IOError
        self.assertRaises(IOError, tcp_client.recv_data_from_socket, mock_socket, data_handlers)

if __name__ == '__main__':
    unittest.main()