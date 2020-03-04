`timescale 1 ns/100 ps

module main_control_block_tb();


	 reg sysClk;
	 reg [7:0] byte_in; // byte from spi interface
	 reg rx_valid;  // is the byte valid?
	 reg [3:0] byte_num;

	 wire[7:0] cam_i2c_byte;
	 wire[1:0] compression;
	 wire RGB;
	 wire cam_id;
	 wire[27:0] timestamp;
	 wire       trigger;
	 wire[15:0] trigger_index;
	 wire hard_reset;
	 wire cam_i2c_output_valid;
	 wire cam_interface_output_valid;


	 wire[71:0] full_buff;
	 assign full_buff = 	72'b000000111010000110001110111111100110111110100110001101100001101010101010;
		//0000,0011 |1010,0001,1000,1110,1111,1110,0110,1111,1010,0110,0011,0110,0001,1010,1010,1010

	 wire[3:0] byte_counter;
	 reg[3:0] byte_counter_s;

	 assign byte_counter = byte_counter_s;

	 FPGA_MainControl_Block dut(
					.sysClk(sysClk),
					.spi_byte(byte_in),
					.spi_input_valid(rx_valid),
					.spi_byte_num(byte_num),
					.cam_i2c_byte_MCB(cam_i2c_byte),
			 	   .compression_MCB(compression),
			 	   .RGB_MCB(RGB),
			 	   .cam_id_MCB(cam_id),
			 	   .timestamp_MCB(timestamp),
			 	   .trigger_MCB(trigger),
			 	   .trigger_index_MCB(trigger_index),
			 	   .cam_i2c_output_valid_MCB(cam_i2c_output_valid),
			 	   .cam_interface_output_valid_MCB(cam_interface_output_valid)
					);


	 initial begin
		byte_num = 4'b0000;
		byte_counter_s = 4'b0000;
		byte_in = 0;
		rx_valid = 0;
		sysClk = 0;
		#10
		rx_valid = 1;
	 end



		always begin

			#10
			sysClk = ~sysClk;



		end

		always@(posedge sysClk) begin
			if(rx_valid) begin
				case (byte_counter)
					4'b0000: begin
						byte_in <= full_buff[71:64];
						byte_num <= byte_counter;
					end
					4'b0001: begin
						byte_in <= full_buff[63:56];
						byte_num <= byte_counter;
					end
					4'b0010: begin
						byte_in <= full_buff[55:48];
						byte_num <= byte_counter;
					end
					4'b0011:  begin
						byte_in <= full_buff[47:40];
						byte_num <= byte_counter;
					end
					4'b0100: begin
					 	byte_in <= full_buff[39:32];
						byte_num <= byte_counter;
					end
					4'b0101:  begin
						byte_in <= full_buff[31:24];
						byte_num <= byte_counter;
					end
					4'b0110:  begin
						byte_in <= full_buff[23:16];
						byte_num <= byte_counter;
					end
					4'b0111:  begin
						byte_in <= full_buff[15:8];
						byte_num <= byte_counter;
					end
					4'b1000: begin
					 	byte_in <= full_buff[7:0];
						byte_num <= byte_counter;
					end
					default:  begin
						byte_in <= 8'h00;
						byte_num <= byte_counter;
						byte_counter_s <= 4'b0000;
						byte_in <= 8'h00;
						rx_valid <= 0;
					end
				endcase

				byte_counter_s <= byte_counter + 4'b0001;

			end
		end


endmodule
