module mem_register_table(input  sysClk, //system clock
							 /* from the intr_data_buffer */
								  input [7:0]  reg_addr,
								  input [16:0] reg_data,
								  input        reg_input_valid,

							 /*	from mem interface block */
								  input [23:0] jpg_size,
								  input        jpg_size_valid,

									input done_reading_img_flag,
									input done_erasing_img_flag,
							 /*   from num_img_stored_reg */
								  input [11:0] num_both_img,
								  input        num_both_img_valid,


					       /* to num_img_stored_reg */
								  output reg		 read_num_img,

							 /*  to mem interface block  */
								  output reg		 read_img_size,


								  output [11:0] img_index,

									output reg	stop_reading_img_flag, //for stop reading procedure
									output     reset_img_pointer_flag,
									output reg start_reading_img_flag, //for reading out an image stored in memory
									output reg		 start_erasing_img_flag, //for erasing an image stored in memory

							/*to SPI interface */
								  output reg [7:0]  byte_out,
								  output        byte_out_valid
								  );

				wire read_img_mode;
				wire erase_img_mode;
				wire numimg_mode;
				wire imgsize_mode;

				reg read_img_mode_s;
				reg erase_img_mode_s;
				reg numimg_mode_s;
				reg imgsize_mode_s;
				
				assign numimg_mode = (reg_addr[3:0] == 4'b1000) & numimg_mode_s;
				assign imgsize_mode = (reg_addr[3:0] == 4'b1001) & imgsize_mode_s;

				assign read_img_mode = (reg_addr[3:0] == 4'b0111) & read_img_mode_s;
				assign erase_img_mode = (reg_addr[3:0] == 4'b1010) & erase_img_mode_s;

				reg[23:0] jpgsize_buffer_s;
				wire[23:0] jpgsize_buffer;

				assign jpgsize_buffer = jpgsize_buffer_s;

				reg[11:0] numimg_buffer_s;
				wire[11:0] numimg_buffer;

				assign numimg_buffer = numimg_buffer_s;

				wire start_outting_jpgsize_flag;
				reg start_outting_jpgsize_flag_s;

				assign start_outting_jpgsize_flag = start_outting_jpgsize_flag_s;
				
				wire start_outting_numimg_flag;
				reg start_outting_numimg_flag_s;

				assign start_outting_numimg_flag = start_outting_numimg_flag_s;

				always@(posedge sysClk) begin
					if(reg_input_valid) begin
						if(numimg_mode) begin
								read_num_img <= 1;
								if(num_both_img_valid) begin
									numimg_buffer_s <= num_both_img;
								end

								numimg_mode_s <= 0;
								start_outting_numimg_flag_s <= 1;
						end
						else if(imgsize_mode) begin
								read_img_size <= 1;
								if(jpg_size_valid) begin
									jpgsize_buffer_s <= jpg_size;
								end

								imgsize_mode_s <= 0;
								start_outting_jpgsize_flag_s <= 1;
						end
						else if(read_img_mode)begin
								start_reading_img_flag <= 1;
								stop_reading_img_flag <= reg_data[17];
						end
						else if(erase_img_mode)begin
								start_erasing_img_flag <= 1;
						end
						else begin
								 read_img_mode_s <= 0;
								 erase_img_mode_s <= 0;
								 numimg_mode_s <= 0;
								 imgsize_mode_s <= 0;
						end
					end

					if(done_erasing_img_flag) begin
							read_img_mode_s <= 0;
					end
					if(done_reading_img_flag) begin
						  erase_img_mode_s <= 0;
					end

				end

				wire[1:0] byte_counter;
				reg[1:0] byte_counter_s;

				reg done_reading_numimg_flag_s;
				wire done_reading_numimg_flag;

				assign done_reading_numimg_flag = done_reading_numimg_flag_s;

				reg done_reading_imgsize_flag_s;
				wire done_reading_imgsize_flag;

				assign done_reading_imgsize_flag = done_reading_imgsize_flag_s;

				always@(posedge sysClk) begin
						if(start_outting_numimg_flag) begin
							case (byte_counter)
									2'b00: byte_out <= numimg_buffer[7:0];
									2'b01: byte_out <= numimg_buffer[15:8];
									2'b10: done_reading_numimg_flag_s <= 1;
									2'b11: done_reading_numimg_flag_s <= 1;
									default: byte_out <= 8'b00000000;
							endcase

							if(done_reading_numimg_flag)begin
								start_outting_numimg_flag_s <= 0;
								done_reading_numimg_flag_s <= 0;
							end
						end

						if(start_outting_jpgsize_flag) begin
							case(byte_counter)
								2'b00: byte_out <= jpgsize_buffer[7:0];
								2'b01: byte_out <= jpgsize_buffer[15:8];
								2'b10: byte_out <= jpgsize_buffer[23:16];
								2'b11: done_reading_imgsize_flag_s <= 1;
								default: byte_out <= 8'b00000000;
							endcase

							if(done_reading_imgsize_flag) begin
									start_outting_jpgsize_flag_s <= 0;
									done_reading_imgsize_flag_s <= 0;
							end
						end
				end


endmodule
