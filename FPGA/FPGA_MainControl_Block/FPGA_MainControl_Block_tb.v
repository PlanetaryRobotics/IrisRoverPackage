`timescale 1 ns/100 ps

module main_control_block_tb();


	 reg sysClk;
	 reg [7:0] byte_in; // byte from spi interface

	 reg rx_valid;  // is the byte valid?

	 wire [7:0]   instruction;
	 wire [63:0] data;
	 wire         valid_buffer_for_camwrite;
	 wire         valid_buffer_for_read;
	 wire         valid_buffer_for_mem;


	 wire[71:0] full_buff;
	 assign full_buff = 	24'h02FFFF;


	 wire[3:0] byte_counter;
	 reg[3:0] byte_counter_s;

	 assign byte_counter = byte_counter_s; 

	 FPGA_MainControl_Block dut(
					.sysClk(sysClk),
					.spi_byte(byte_in),
					.spi_input_valid(rx_valid),
					.instruction_MCB(instruction),
					.data_MCB(data),
					.valid_buffer_for_camwrite_MCB(valid_buffer_for_camwrite),
					.valid_buffer_for_read_MCB(valid_buffer_for_read),
					.valid_buffer_for_mem_MCB(valid_buffer_for_mem)
					);

	//reg initiate;

	 initial begin
		byte_counter_s = 4'b0000;
		byte_in = 0;
		rx_valid = 0;
		sysClk = 0;
		//initiate = 0;
	 end

		
		
		always begin
			/*if(~initiate) begin
				#10
				sysClk = 1;
				#10
				sysClk= 0;
				initiate = ~initiate;
			end*/
			#10
			sysClk = ~sysClk;
			

			
		end

		always@(posedge sysClk) begin
			//if(initiate) begin
				byte_counter_s <= byte_counter + 1;
		//	end
			rx_valid = 1;
			//if(initiate) begin
				case (byte_counter)
					4'b0000: byte_in <= full_buff[23:16];
					4'b0001: byte_in <= full_buff[15:8];
					4'b0010: byte_in <= full_buff[7:0];
//					4'b0011: byte_in <= full_buff[15:8];
//					4'b0100: byte_in <= full_buff[7:0];
//					4'b0101: byte_in <= full_buff[31:24];
//					4'b0110: byte_in <= full_buff[23:16];
//					4'b0111: byte_in <= full_buff[15:8];
//					4'b1000: byte_in <= full_buff[7:0];
				endcase
			//end

			if(byte_counter == 4'b0101) begin
				byte_counter_s = 4'b0000;
				rx_valid = 0;
				//initiate = 0;
			end
		end


endmodule
