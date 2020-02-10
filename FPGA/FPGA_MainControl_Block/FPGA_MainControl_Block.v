module FPGA_MainControl_Block(input sysClk,
														/*for instr_data_buffer */
														input [7:0] spi_byte, //from spi block
														input       spi_input_valid, //from spi block

														/*for status_reg */
														input proc_img_1_flag, //from cam interface block
														input proc_img_2_flag, //from cam interface block
														input erase_img_flag, //from mem interface block
														input booting_cam_flag, //from cam interface block

														/*for error_table */
														input [15:0] index_of_error, //from mem interface block
														input 			 camid_of_error, //from cam interface block
														input 		   cam_timeout_error_flag, //from cam interface block
														input        cap_failure_flag, //from cam interface block
														input   	   cam_undetected_flag, // from cam interface block
														input        write_fail_flag, //from cam interface block
														input        read_fail_flag, //from cam interface block
														input        erase_fail_flag, //from mem interface block

														/*for mem_register_table */
														input [23:0] jpg_size_MCB, //from mem interface block
														input        jpg_size_valid_MCB, //from mem interface block
														input        done_reading_img_flag_MCB, //from mem interface block
														input        done_erasing_img_flag_MCB, //from mem interface block

														/*for num_img_stored_reg */
														input nav_img_added_flag_MCB, // from mem interface block
														input science_img_added_flag_MCB, //from mem interface block

														/*for cam_write_register_table */
														input ready_for_next_byte_MCB,

														/*currently testing outputs for instr_data_buffer */
														output [7:0]  instruction_MCB,
														output [63:0] data_MCB,
														output        valid_buffer_for_camwrite_MCB,
														output        valid_buffer_for_read_MCB,
														output        valid_buffer_for_mem_MCB

								  );
								wire [7:0]  instruction;
								wire [63:0] data;
								wire valid_buffer_for_camwrite;
								wire valid_buffer_for_read;
								wire valid_buffer_for_mem; 
								
									instr_data_buffer idb_test(
													sysClk,
													spi_byte,
													spi_input_valid,

													instruction,
													data,
													valid_buffer_for_camwrite,
													valid_buffer_for_read,
													valid_buffer_for_mem 
										);
										
								assign instruction_MCB = instruction;
								assign data_MCB = data;
								assign valid_buffer_for_camwrite_MCB = valid_buffer_for_camwrite;
								assign valid_buffer_for_read_MCB = valid_buffer_for_read;
								assign valid_buffer_for_mem_MCB = valid_buffer_for_mem; 
									


endmodule
