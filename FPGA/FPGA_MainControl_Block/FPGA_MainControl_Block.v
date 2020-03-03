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
														output        valid_buffer_for_mem_MCB,

														/*currently testing outputs from cam_write_register_table */
														output[7:0] cam_i2c_byte_MCB,
														output[1:0] compression_MCB,
														output RGB_MCB,
														output cam_id_MCB,
														output[27:0] timestamp_MCB,
														output       trigger_MCB,
														output[15:0] trigger_index_MCB,
														output soft_reset_MCB,
														output hard_reset_MCB,
														output output_valid_MCB
								  );
								wire [7:0]  instruction;
								wire [63:0] data;
								wire valid_buffer_for_camwrite;
								wire valid_buffer_for_read;
								wire valid_buffer_for_mem;


								wire[7:0] cam_i2c_byte;
								wire[1:0] compression;
								wire RGB;
								wire cam_id;
								wire[27:0] timestamp;
								wire       trigger;
								wire[15:0] trigger_index;
								wire soft_reset;
								wire hard_reset;
								wire output_valid;


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

										cam_write_register_table cwrt_test(
													sysClk,
													instruction,
													data,
													valid_buffer_for_camwrite,

													cam_i2c_byte,
													compression,
												  RGB,
												  cam_id,
												  timestamp,
													trigger,
													trigger_index,
													soft_reset,
													hard_reset,
													output_valid
											);


								assign instruction_MCB = instruction;
								assign data_MCB = data;
								assign valid_buffer_for_camwrite_MCB = valid_buffer_for_camwrite;
								assign valid_buffer_for_read_MCB = valid_buffer_for_read;
								assign valid_buffer_for_mem_MCB = valid_buffer_for_mem;


								assign cam_i2c_byte_MCB = cam_i2c_byte;
								assign compression_MCB = compression;
								assign RGB_MCB = RGB;
								assign cam_id_MCB = cam_id;
						  	assign 	timestamp_MCB = timestamp;
								assign trigger_MCB = trigger;
								assign trigger_index_MCB = trigger_index;
								assign soft_reset_MCB = soft_reset;
							  assign 	hard_reset_MCB = hard_reset;
						  	assign 	output_valid_MCB = output_valid;
endmodule
