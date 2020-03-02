module FPGA_Memory_Interface( input sysClk,
															/*From Main Control Block */
															input[15:0] image_index,
															input read_image_size,
															input start_reading_image_flag,
															input stop_reading_image_flag,
															input reset_image_pointer_flag,
															input start_erasing_image_flag,

															/*From JPEG Block */
															input[7:0] byte_to_store,
															input  valid_byte_flag,

															/*From SPI Interface*/
															input ready_for_image_byte,

															/*From Flash Memory*/
															input MISO,


															/*To Main Control Block */
															output navigation_image_added_flag,
															output science_image_added_flag,
															output[31:0] JPEG_size,
															output JPG_image_size_valid_input,
															output done_reading_flag,
															output done_erasing_flag,
															output image_index_of_error_flag,
															output write_failure_error_flag,
															output read_failure_error_flag,
															output erase_failure_error_flag,
															output error_flag_valid_flag,

															/*To SPI Interface */
															output [7:0] image_byte_to_spi,
															output valid_flag_for_img_byte,

															/* To Flash Memory */
															output SPICLK,
															output MOSI,
															output CS
														);


endmodule
