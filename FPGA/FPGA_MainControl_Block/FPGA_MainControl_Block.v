module FPGA_MainControl_Block(input sysClk,
														/*for instr_data_buffer */
														input [7:0] spi_byte, //from spi block
														input       spi_input_valid, //from spi block
														input [3:0] spi_byte_num,
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
														output[10:0] upper_x_val,
														output[11:0] upper_y_val,
														output[10:0] img_height,
														output[11:0] img_width,
														output hard_reset_MCB,
														output cam_i2c_output_valid_MCB,
														output cam_interface_output_valid_MCB,

														/*currently testing outputs from mem_register_table */
														 output read_img_size_MCB,
														 output [11:0] img_index_MCB,
														 output stop_reading_img_flag_MCB,
														 output reset_img_pointer_flag_MCB,
														 output start_reading_img_flag_MCB,
														 output start_erasing_img_flag_MCB,
														 output [7:0]  byte_out_MCB,
														 output byte_out_valid_MCB
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
													spi_byte_num,

													instruction,
													data,
													valid_buffer_for_camwrite,
													valid_buffer_for_read,
													valid_buffer_for_mem
										);



										 cam_write_register_table cwrt_test(
										   .sysClk(sysClk),
										   .reg_addr(instruction),
										   .reg_data(data),
										   .intr_valid_input(valid_buffer_for_camwrite),
											.ready_for_next_byte(ready_for_next_byte_MCB),
										   .cam_i2c_byte_out(cam_i2c_byte_MCB),
										   .compression(compression_MCB),
										   .RGB(RGB_MCB),
										   .cam_id(cam_id_MCB),
										   .timestamp(timestamp_MCB),
										   .trigger(trigger_MCB),
										   .trigger_index(trigger_index_MCB),
										   .hard_reset(hard_reset_MCB),
										   .cam_i2c_output_valid(cam_i2c_output_valid_MCB),
										   .cam_interface_output_valid(cam_interface_output_valid_MCB)
										 	);

										wire begin_numimg_flush;
										wire [31:0] out_numimg;
										wire out_numimg_valid;

										num_img_stored_reg nisr(
												nav_img_added_flag_MCB,
												science_img_added_flag_MCB,
												begin_numimg_flush,

												out_numimg,
												out_numimg_valid
											);


										mem_register_table mrt(
														sysClk,
														instruction,
														data,
														valid_buffer_for_mem,
														jpg_size_MCB,
														jpg_size_valid_MCB,
														done_reading_img_flag_MCB,
														done_erasing_img_flag_MCB,
													  out_numimg,
														out_numimg_valid,

														begin_numimg_flush,
														read_img_size_MCB,
														img_index_MCB,
														stop_reading_img_flag_MCB,
														reset_img_pointer_flag_MCB,
														start_reading_img_flag_MCB,
														start_erasing_img_flag_MCB,
														byte_out_MCB,
														byte_out_valid_MCB
											);



endmodule
