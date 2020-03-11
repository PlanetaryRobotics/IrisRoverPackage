/* compression amount -> cam interface
	 RGB or Grayscale? -> cam interface
	 Exposure Value -> set of instr for img sensor
	 Shutter Speed  -> set of instr for img sensor
	 Cropping Amount  -> set of instr for img sensor
	 Row bin  -> set of instr for img sensor
	 Column bin  -> set of instr for img sensor
*/
module cam_i2c_command_map(
						       input[7:0]   reg_addr,
						       input[63:0]  reg_data,
									 input        valid_input,
									 input[4:0]   byte_counter,

									 output [7:0] cam_i2c_byte,
									 output reg       cam_i2c_output_valid,
									 output reg    all_bytes_out
						 );

				reg[7:0] byte_out_r;

				assign cam_i2c_byte = byte_out_r;

				always@(*) begin
				  if(valid_input) begin
						all_bytes_out = 0;
						if((reg_addr == 8'h03) | (reg_addr == 8'h04))begin //shutter width upper, shutter width lower, and shutter delay
								case(byte_counter)
									5'b00000: byte_out_r = 8'h08; //shutter width upper reg addr
									5'b00001: byte_out_r = 8'h00;
									5'b00010: byte_out_r = {4'b0000, reg_data[22:19]};

									5'b00011: byte_out_r = 8'h09; //shutter width lower reg addr
									5'b00100: byte_out_r = reg_data[18:11];
									5'b00101: byte_out_r = reg_data[10:3];

									5'b00110: byte_out_r = 8'h0C; //shutter delay addr
									5'b00111: byte_out_r = {3'b000, reg_data[35:31]};
									5'b01000: byte_out_r = reg_data[30:23];

									5'b01001: byte_out_r = 8'h22; // Row Address Mode (for Row Bin)
									5'b01010: byte_out_r = 8'h00;
									5'b01011: byte_out_r = {2'b00, reg_data[37:36], 4'h0};

									5'b01100: byte_out_r = 8'h23; //Column Address Mode (for Col Bin)
									5'b01101: byte_out_r = 8'h00;
									5'b01110: byte_out_r = {2'b00, reg_data[39:38], 4'h0};

									5'b01111: byte_out_r = 8'h05; //Horizontal Blanking
									5'b10000: byte_out_r = {4'h0, reg_data[51:48]};
									5'b10001: byte_out_r = reg_data[47:40];

									5'b10010: byte_out_r = 8'h06; //Vertical Blanking
									5'b10011: byte_out_r = {5'b00000, reg_data[62:60]};
									5'b10100: byte_out_r = reg_data[59:52];

									default: byte_out_r = 8'h00;
								endcase

								cam_i2c_output_valid = 1;
								if(byte_counter == 5'b10100) begin
									all_bytes_out = 1;
								end
					  end //if((reg_addr == 8'h02) | (reg_addr == 8'h03))

						else if((reg_addr == 8'h05) | (reg_addr == 8'h06)) begin
								case(byte_counter)
									5'b00000: byte_out_r = 8'h01; //Row Start
									5'b00001: byte_out_r = {5'b00000, reg_data[10:8]};
									5'b00010: byte_out_r = reg_data[7:0];

									5'b00011: byte_out_r = 8'h02; //Column Start
									5'b00100: byte_out_r = {4'b0000, reg_data[22:19]};
									5'b00101: byte_out_r = reg_data[18:11];

									5'b00110: byte_out_r = 8'h03; // Row size
									5'b00111: byte_out_r = {5'b00000, reg_data[33:31]};
									5'b01000: byte_out_r = reg_data[30:23];

									5'b01001: byte_out_r = 8'h04; //column size
									5'b01010: byte_out_r = {4'b0000, reg_data[45:42]};
									5'b01011: byte_out_r = reg_data[41:34];

									default: byte_out_r = 8'h00;
								endcase

								cam_i2c_output_valid = 1;
								if(byte_counter == 5'b01011) begin
									all_bytes_out = 1;
								end
						end //else if((reg_addr == 8'h05) | (reg_addr == 8'h06))
						else if(reg_addr == 8'h0B)begin
							case(byte_counter)
								5'b00000: byte_out_r = 8'h0D;
								5'b00001: byte_out_r = 8'h00;
								5'b00010: byte_out_r = {7'b0000000, reg_data[1]};

								default: byte_out_r = 8'h00;
							endcase

							cam_i2c_output_valid = 1;
							if(byte_counter == 5'b00010) begin
								all_bytes_out = 1;
							end
						end //else if(reg_addr == 8'h0B
						else begin
							byte_out_r = 8'h00;
							all_bytes_out = 0;
							cam_i2c_output_valid = 0;
						end //else
					end //if(valid_input)
					else begin
						byte_out_r = 8'h00;
						all_bytes_out = 0;
						cam_i2c_output_valid = 0;
					end
				end



endmodule


module cam_write_register_table(input           sysClk, //clock
										  /*from instruction buffer */
										  input [7:0]     reg_addr,
										  input [63:0]   reg_data,
										  input    		   intr_valid_input,

											/*from cam interface */
											input ready_for_next_byte,


										  /* cam i2c stuff */
										  output [7:0]  cam_i2c_byte_out,

										  /* header stuff */
										  output [1:0]  compression,
										  output        RGB,
										  output        cam_id,
											output [27:0]  timestamp,
											output[15:0] 	trigger_index,
											output[10:0] upper_x_val,
											output[11:0] upper_y_val,
											output[10:0] img_height,
											output[11:0] img_width,

											/* cam stuff*/
											output 				trigger,
											output hard_reset,

										  output cam_i2c_output_valid,
											output cam_interface_output_valid
										  );

					reg cam_i2c_output_valid_r;
					reg cam_interface_output_valid_r;
					reg cam_id_r;
					reg [1:0] compression_r;
					reg RGB_r;
					reg [7:0] cam_i2c_byte_out_r;
					reg trigger_r;
					reg[15:0] trigger_index_r;
					reg[10:0] upper_x_val_s,
					reg[11:0] upper_y_val_s,
					reg[10:0] img_height_s,
					reg[11:0] img_width_s,
					reg[27:0] timestamp_r;

					assign cam_i2c_output_valid = cam_i2c_output_valid_r;
					assign cam_interface_output_valid = cam_interface_output_valid_r;
					assign cam_id = cam_id_r;
					assign compression = compression_r;
					assign RGB = RGB_r;
					assign trigger_index = trigger_index_r;
					assign trigger = trigger_r;
					assign timestamp = timestamp_r;
					assign upper_x_val = upper_x_val_s;
					assign upper_y_val = upper_y_val_s;
					assign img_height = img_height_s;
					assign img_width = img_width_s;


					wire [4:0] byte_counter_w;
					reg  [4:0] byte_counter_r;

					assign byte_counter_w = byte_counter_r;

					/* i/o for the command map */
					wire 		valid_input_for_commandmap;
					wire[7:0] byte_out_from_commandmap;
					wire byte_valid_out_from_commandmap;
					wire all_byte_out_flag;

					reg valid_input_for_commandmap_r;

					assign valid_input_for_commandmap = valid_input_for_commandmap_r;

					cam_i2c_command_map cicm(
													reg_addr,
													reg_data,
													valid_input_for_commandmap,
													byte_counter_w,

													cam_i2c_byte_out,
													byte_valid_out_from_commandmap,
													all_byte_out_flag
										);


					wire bytes_vals_valid;
					reg bytes_vals_valid_r;

					assign bytes_vals_valid = bytes_vals_valid_r;


					wire[2:0] running_buffer;
					reg[2:0] running_buffer_s;

					assign running_buffer = running_buffer_s;

					wire running;
					reg running_s;

					assign running = running_s;

					wire stopping;
					reg stopping_s;

					assign stopping = stopping_s;

					initial begin
						 cam_i2c_output_valid_r <= 0;
						 cam_interface_output_valid_r <= 0;
						 cam_id_r <= 0;
						 compression_r <= 0;
						 RGB_r <= 0;
					 	 cam_i2c_byte_out_r <= 0;
						 trigger_r <= 0;
						 trigger_index_r = 0;
						 timestamp_r <= 0;
						 byte_counter_r <= 0;
						 valid_input_for_commandmap_r <= 0;
						 bytes_vals_valid_r <= 0;
						 running_buffer_s <= 0;
						 running_s <= 0;
						 stopping_s <= 0;
					end


					always@(posedge sysClk) begin
							running_buffer_s <= {running_buffer[1:0], intr_valid_input};
							if(running_buffer == 2'b01) begin
								running_s <= 1;
								stopping_s <= 0;
								byte_counter_r <= 0;
							end


							if(running_s)begin
								if(reg_addr == 8'h01) begin
										trigger_r <= 1;
										cam_id_r <= reg_data[0];
										trigger_index_r <= reg_data[16:1];
										timestamp_r <= reg_data[44:17];
										cam_interface_output_valid_r <= 1;
										cam_i2c_output_valid_r <= 0;
										stopping_s <= 1;
										cam_interface_output_valid_r <= 1;
								end
								else if((reg_addr == 8'h03) | (reg_addr == 8'h04))begin
									if(all_byte_out_flag) begin
										stopping_s <= 1;
										valid_input_for_commandmap_r <= 0;
									end
									else begin
										cam_id_r <= (reg_addr == 8'h04);
										cam_interface_output_valid_r <= 1;
										cam_i2c_output_valid_r <= 1;
										compression_r = reg_data[1:0];
										RGB_r <= reg_data[2];
										valid_input_for_commandmap_r <= 1;

										if(byte_valid_out_from_commandmap)begin
											cam_i2c_byte_out_r <= byte_out_from_commandmap;
											byte_counter_r <= byte_counter_w + 5'b00001;
										end

									end
								end
								else if((reg_addr == 8'h05) | (reg_addr == 8'h06))begin
									if(all_byte_out_flag) begin
										stopping_s <= 1;
										valid_input_for_commandmap_r <= 0;
									end
									else begin
											cam_id_r <= (reg_addr == 8'h06);
											cam_interface_output_valid_r <= 1;
											cam_i2c_output_valid_r <= 1;
											upper_x_val_s <= reg_data[10:0];
											upper_y_val_s <= reg_data[22:11];
											img_height_s <= reg_data[33:23];
											img_width_s <= reg_data[45:34];

											valid_input_for_commandmap_r <= 1;
											if(byte_valid_out_from_commandmap)begin
												cam_i2c_byte_out_r <= byte_out_from_commandmap;
												byte_counter_r <= byte_counter_w + 5'b00001;
											end

										end
								end
								else if(reg_addr == 8'h0B) begin
									if(all_byte_out_flag) begin
										stopping_s <= 1;
										valid_input_for_commandmap_r <= 0;
									end
									else begin
										cam_id_r <= (reg_addr[0]);
										cam_interface_output_valid_r <= 0;
										cam_i2c_output_valid_r <= 1;
										valid_input_for_commandmap_r <= 1;
										if(byte_valid_out_from_commandmap)begin
											cam_i2c_byte_out_r <= byte_out_from_commandmap;
											byte_counter_r <= byte_counter_w + 5'b00001;
										end
									end
								end
								else begin

								end

								if(stopping)begin
									running_s <= 0;
									cam_interface_output_valid_r <= 0;
									cam_i2c_output_valid_r <= 0;
									valid_input_for_commandmap_r <= 0;
								end

							end
					end

endmodule
