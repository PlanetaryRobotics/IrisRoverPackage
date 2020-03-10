import unittest
import select
import socket 
from teleop_backend.network import message_parsing_state_machine
import multiprocessing
from teleop_backend.network import multiprocess_tcp_client
from unittest import mock

class Test_recv_data_from_socket(unittest.TestCase):

	def test_emptyDataHandler(self):
		"""
		Checks for when data_handlers is empty and there is data
	    
	    Expected Behavior: Raises warning
		"""
		data_handlers = []
		with unittest.mock.patch("socket.socket") as mock_socket:
			mock_shutdown_event = unittest.mock.MagicMock()
			tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(mock_shutdown_event)
			mock_socket.recv.return_value = b"my_test_data"
			tcp_client.recv_data_from_socket(mock_socket, data_handlers)
		self.assertWarns(Warning)

	def test_noData(self):
		"""
		Checks for when no data from socket
	    
	    Expected Behavior: returns False
		"""
		completed_msg_queue = multiprocessing.Queue()
		msg_parser = message_parsing_state_machine.MessageParsingStateMachine(input_bytes_will_be_big_endian=True)
		msg_parser.register_completed_message_queue(completed_message_queue=completed_msg_queue)
		data_handlers = [msg_parser]

		with unittest.mock.patch("socket.socket") as mock_socket:
			mock_shutdown_event = unittest.mock.MagicMock()
			tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(mock_shutdown_event)
			mock_socket.recv.return_value = b""
		self.assertEqual(tcp_client.recv_data_from_socket(mock_socket, data_handlers), False)

	def test_existsData(self):
		"""
		Checks for when data exists from socket
	    
	    Expected Behavior: returns True
		"""
		completed_msg_queue = multiprocessing.Queue()
		msg_parser = message_parsing_state_machine.MessageParsingStateMachine(input_bytes_will_be_big_endian=True)
		msg_parser.register_completed_message_queue(completed_message_queue=completed_msg_queue)
		data_handlers = [msg_parser]
		
		with unittest.mock.patch("socket.socket") as mock_socket:
			mock_shutdown_event = unittest.mock.MagicMock()
			tcp_client = multiprocess_tcp_client.MultiprocessTcpClient(mock_shutdown_event)
			mock_socket.recv.return_value = b"sample data"
		self.assertEqual(tcp_client.recv_data_from_socket(mock_socket, data_handlers), True)

if __name__ == '__main__':
	unittest.main()