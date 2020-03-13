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
	 //assign full_buff = 	72'b000000111010000110001110111111100110111110100110001101100001101010101110;
		//0000,0011 |1010,0001,1000,1110,1111,1110,0110,1111,1010,0110,0011,0110,0001,1010,1010,1110

	 assign full_buff = 	72'b000000010000000000000000000111110101100110010101100110010101010010101011;
	 //0000,0001 | 1101,0101,0010,1010,1001,1001,1010,1001,1001,1010,1111,1000,0000,0000,0000,0000

	 //assign full_buff = 	72'b000001100000000000000000001010101010100001110011000000000001100000001001;
	 //0000, 0110 | 0000, 0000,0000,0000,0010,1010,1010,1000,0111,0011,0000,0000,0001,1000,0000,1001

	 //assign full_buff = 72'b000010000000000000000000000000000000000000000000000000000000000000000000;
	 //0000,1000 | 0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000

//	 reg nav_img_added_flag_r; // from mem interface block
//	 reg science_img_added_flag_r; //from mem interface block
	
	 wire[3:0] byte_counter;
	 reg[3:0] byte_counter_s;

	 assign byte_counter = byte_counter_s;

//	 	wire [23:0] jpg_size; //from mem interface block
//		wire jpg_size_valid; //from mem interface block
//		wire done_reading_img_flag; //from mem interface block
//		wire done_erasing_img_flag; //from mem interface block
//		wire nav_img_added_flag; // from mem interface block
//		wire science_img_added_flag; //from mem interface block
//
//		wire read_img_size;
//		wire [11:0] img_index;
//		wire stop_reading_img_flag;
//		wire reset_img_pointer_flag;
//		wire start_reading_img_flag;
//		wire start_erasing_img_flag;
//		wire [7:0]  byte_out;
//		wire byte_out_valid;

			
//		assign nav_img_added_flag = nav_img_added_flag_r;
//		assign science_img_added_flag = science_img_added_flag_r;
		
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
					.hard_reset_MCB(hard_reset),
					.cam_i2c_output_valid_MCB(cam_i2c_output_valid),
					.cam_interface_output_valid_MCB(cam_interface_output_valid)

//					.jpg_size_MCB(jpg_size),
//					.jpg_size_valid_MCB(jpg_size_valid),
//					.done_reading_img_flag_MCB(done_reading_img_flag),
//					.done_erasing_img_flag_MCB(done_erasing_img_flag),
//					.nav_img_added_flag_MCB(nav_img_added_flag),
//					.science_img_added_flag_MCB(science_img_added_flag),
//
//					.read_img_size_MCB(read_img_size),
//					.img_index_MCB(img_index),
//					.stop_reading_img_flag_MCB(stop_reading_img_flag),
//					.reset_img_pointer_flag_MCB(reset_img_pointer_flag),
//					.start_reading_img_flag_MCB(start_reading_img_flag),
//					.start_erasing_img_flag_MCB(start_erasing_img_flag),
//					.byte_out_MCB(byte_out),
//					.byte_out_valid_MCB(byte_out_valid)
					);


	 initial begin
//	  nav_img_added_flag_r = 1;
//		science_img_added_flag_r = 0;
//		#10
//		nav_img_added_flag_r = 1;
//		science_img_added_flag_r = 0;
//		#10
//		nav_img_added_flag_r = 1;
//		science_img_added_flag_r = 0;
//		#10
//		nav_img_added_flag_r = 0;
//		science_img_added_flag_r = 1;
//		#10
//		nav_img_added_flag_r = 1;
//		science_img_added_flag_r = 0;
//		#10
//		nav_img_added_flag_r = 0;
//		science_img_added_flag_r = 0;

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
