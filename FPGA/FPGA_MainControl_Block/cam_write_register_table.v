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
									 output        cam_i2c_output_valid,
									 output        all_bytes_out
						 );

				reg[7:0] byte_out_r;

				assign cam_i2c_byte = byte_out_r;

				always@(*) begin
				  if(valid_input) begin
						all_bytes_out = 0;
						if((reg_addr == 8'h02) | (reg_addr == 8'h03))begin //shutter width upper, shutter width lower, and shutter delay
								case(byte_counter) begin
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
								case(byte_counter) begin
									5'b00000: byte_out_r = 8'h01; //Row Start
									5'b00001: byte_out_r = {5'b00000, reg_addr[10:8]};
									5'b00010: byte_out_r = reg_addr[7:0];

									5'b00011: byte_out_r = 8'h02; //Column Start
									5'b00100: byte_out_r = {4'b0000, reg_addr[22:19]};
									5'b00101: byte_out_r = reg_addr[18:11];

									5'b00110: byte_out_r = 8'h03; // Row size
									5'b00111: byte_out_r = {5'b00000, reg_addr[33:31]};
									5'b01000: byte_out_r = reg_addr[30:23];

									5'b01001: byte_out_r = 8'h04; //column size
									5'b01010: byte_out_r = {4'b0000, reg_addr[45:42]};
									5'b01011: byte_out_r = reg_addr[41:34];

									default: byte_out_r = 8'h00;
								endcase

								cam_i2c_output_valid = 1;
								if(byte_counter == 5'b01011) begin
									all_bytes_out = 1;
								end
						end //else if((reg_addr == 8'h05) | (reg_addr == 8'h06))
						else begin

						end //else
					end //if(valid_input)
					else begin
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
											input ready_for_next_byte;


										  /* cam i2c stuff */
										  output [7:0]  cam_i2c_byte_out,

										  /* cam interface config stuff */
										  output [1:0]  compression,
										  output        RGB,

										  /*all cam stuff*/
										  output        cam_id,
											output [9:0]  timestamp,
											output 				trigger,
											output[15:0] 	trigger_index
										  output        output_valid
										  );

					reg output_valid_r;
					reg cam_id_r;
					reg [1:0] compression_r;
					reg RGB_r;
					reg [7:0] cam_i2c_byte_out_r;
					reg trigger_r;
					reg[15:0] trigger_index_r;
					reg[9:0] timestamp_r;

					assign output_valid = output_valid_r;
					assign cam_id = cam_id_r;
					assign compression = compression_r;
					assign RGB = RGB_r;
					assign cam_i2c_byte_out = cam_i2c_byte_out_r;
					assign trigger_index = trigger_index_r;
					assign trigger = trigger_r;
					assign timestamp_r = timestamp;


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

													byte_out_from_commandmap,
													byte_valid_out_from_commandmap,
													all_byte_out_flag
										);

					wire other_vals_valid;
					reg other_vals_valid_r;

					assign other_vals_valid = other_vals_valid_r;

					wire bytes_vals_valid;
					reg bytes_vals_valid_r;

					assign bytes_vals_valid = bytes_vals_valid_r;

					assign output_valid = ((~reg_addr[2]) & bytes_vals_valid & other_vals_valid) | bytes_vals_valid;


					always@(posedge sysClk) begin
							if(intr_valid_input) begin
								byte_counter_r <= 0;
								valid_input_for_commandmap_r <= 1;
								if(byte_valid_out_from_commandmap) begin
									valid_input_for_commandmap_r <= 0;
									 cam_i2c_byte_out_r <= byte_out_from_commandmap;
									 cam_id_r <= ((reg_addr == 8'h03) | (reg_addr == 8'h06));
									 bytes_vals_valid_r <= 1;
								end

								byte_counter_r <= byte_counter_w + 1;
								bytes_vals_valid_r <= 0;
							end
					end


					always@(posedge sysClk) begin
							if(intr_valid_input) begin
								if(~reg_addr[2]) begin
									RGB_r <= reg_data[2];
									compression_r <= reg_data[1:0];
									other_vals_valid_r <= 1;
								end
								else begin
									RGB_r <= 0;
									compression_r <= 0;
									other_vals_valid_r <= 0;
								end

								if(reg_addr == 8'h01) begin
									trigger_r <= 1;
									trigger_index_r <= reg_data[16:1];
									timestamp_r <= reg_data[26:44];
									cam_id_r <= reg_data[0];
									bytes_vals_valid_r <= 1;
								end
							end
					end



endmodule
