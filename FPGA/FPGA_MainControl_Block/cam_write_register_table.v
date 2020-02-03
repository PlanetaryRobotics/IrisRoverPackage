/* compression amount -> cam interface
	 RGB or Grayscale? -> cam interface
	 Exposure Value -> set of instr for img sensor
	 Shutter Speed  -> set of instr for img sensor
	 Cropping Amount  -> set of instr for img sensor
	 Row bin  -> set of instr for img sensor
	 Column bin  -> set of instr for img sensor
*/

module cam_val_converter(input[2:0] val_type,
								 input[2:0] step,
								 input[31:0] val_in,

								 output[15:0] val_out
								 );


endmodule


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
						if((reg_addr == 8'h02) | (reg_addr == 8'h03))begin //shutter width upper, shutter width lower, and shutter delay
								case(byte_counter) begin
									5'b00000: byte_out_r <= 8'h08; //shutter width upper reg addr
									5'b00001: byte_out_r <= 8'h00;
									5'b00010: byte_out_r <= {4'b0000, reg_data[22:19]};

									5'b00011: byte_out_r <= 8'h09; //shutter width lower reg addr
									5'b00100: byte_out_r <= reg_data[18:11];
									5'b00101: byte_out_r <= reg_data[10:3];

									5'b00110: byte_out_r <= 8'h0C; //shutter delay addr
									5'b00111: byte_out_r <= {3'b000, reg_data[35:31]};
									5'b01000: byte_out_r <= reg_data[30:23];

									5'b01001: byte_out_r <= 8'h22; // Row Address Mode (for Row Bin)
									5'b01010: byte_out_r <= ;
									5'b01011: byte_out_r <= ;

									5'b01100: byte_out_r <= 8'h23; //Column Address Mode (for Col Bin)
									5'b01101: byte_out_r <= ;
									5'b01110: byte_out_r <= ;

									5'b01111: byte_out_r <= 8'h05; //Horizontal Blanking
									5'b1001: byte_out_r <= ;
									5'b1001: byte_out_r <= ;

									5'b1001: byte_out_r <= 8'h06; //Vertical Blanking
									5'b1001: byte_out_r <= ;
									5'b1001: byte_out_r <= ;


									default: byte_out_r <= ;
								endcase
						end

						else if((reg_addr == 8'h05) | (reg_addr == 8'h06)) begin
						case(byte_counter) begin
							4'b0000: byte_out_r <= 8'h01; //Row Start
							4'b0001: byte_out_r <= ;
							4'b0010: byte_out_r <= ;
							4'b0011: byte_out_r <= 8'h02; //Column Start
							4'b0100: byte_out_r <= ;
							4'b0101: byte_out_r <= ;
							4'b0110: byte_out_r <= 8'h03; // Row size
							4'b0111: byte_out_r <= ;
							4'b1000: byte_out_r <= ;
							4'b1001: byte_out_r <= 8'h04; //column size
							4'b1010: byte_out_r <= ;
							4'b1011: byte_out_r <= ;
							default: byte_out_r <= ;
						endcase
						end
						else begin

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
											output 				trigger,
											output[15:0] 	trigger_index
										  output        output_valid
										  );

					reg output_valid_r;
					reg cam_id_r;
					reg [1:0] compression_r;
					reg RGB_r;
					reg [7:0] cam_i2c_byte_out_r;


					assign output_valid = output_valid_r;
					assign cam_id = cam_id_r;
					assign compression = compression_r;
					assign RGB = RGB_r;
					assign cam_i2c_byte_out = cam_i2c_byte_out_r;

					wire [4:0] byte_counter_w;
					reg  [4:0] byte_counter_r;

					assign byte_counter_w = byte_counter_r;

					/* i/o for the command map */
					wire 		valid_input_for_commandmap;
					wire[7:0] byte_out_from_commandmap;
					wire byte_valid_out_from_commandmap;
					wire all_instr_out_flag;


					always@(posedge clock) begin
							if(intr_valid_input) begin
								cam_id_r <= (reg_addr == 8'h02);
								if() begin

								end
								if(~reg_addr[2]) begin //for the configs
										compression_r <= reg_data[1:0];
										RGB_r <= reg_data[2];
								end
								else begin //for the crops

								end
							end
					end





endmodule
