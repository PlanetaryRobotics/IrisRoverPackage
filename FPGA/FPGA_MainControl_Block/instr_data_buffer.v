module instr_data_buffer(input sysClk,
								 input [7:0] byte_in, // byte from spi interface
								 input rx_valid,  // is the byte valid?
								 input [3:0] byte_num, //which byte is this?

								 output  [7:0]   instruction,
								 output  [63:0]  data,
								 output          valid_buffer_for_camwrite,
								 output          valid_buffer_for_read,
								 output          valid_buffer_for_mem
								 );


		wire camwrite_cmd;
		wire read_cmd;
		wire mem_cmd;

		reg[7:0] instruction_s;
		reg[63:0] data_s;
		
		initial begin
			instruction_s = 8'h00;
			data_s = 64'h0000000000000000;
		end
		assign instruction = ((byte_num == 4'b0000) & (rx_valid)) ? byte_in : instruction_s;


   	assign data[63:56] = ((byte_num == 4'b0001) & (rx_valid)) ? byte_in : data_s[63:56];
		assign data[55:48] = ((byte_num == 4'b0010) & (rx_valid)) ? byte_in : data_s[55:48];
		assign data[47:40] = ((byte_num == 4'b0011) & (rx_valid)) ? byte_in : data_s[47:40];
		assign data[39:32] = ((byte_num == 4'b0100) & (rx_valid)) ? byte_in : data_s[39:32];
		assign data[31:24] = ((byte_num == 4'b0101) & (rx_valid)) ? byte_in : data_s[31:24];
		assign data[23:16] = ((byte_num == 4'b0110) & (rx_valid)) ? byte_in : data_s[23:16];
		assign data[15:8] = ((byte_num == 4'b0111) & (rx_valid)) ? byte_in : data_s[15:8];
		assign data[7:0] = ((byte_num == 4'b1000) & (rx_valid)) ? byte_in : data_s[7:0];

		assign valid_buffer_for_camwrite = ((byte_num == 4'b1000) & (rx_valid)) ? camwrite_cmd : 1'b0;
		assign valid_buffer_for_read = ((byte_num == 4'b1000) & (rx_valid)) ? read_cmd : 1'b0;
		assign valid_buffer_for_mem = ((byte_num == 4'b1000) & (rx_valid)) ? mem_cmd : 1'b0;


		assign camwrite_cmd = (instruction == 8'h01) | (instruction == 8'h03) | (instruction == 8'h04) | (instruction == 8'h05) | (instruction == 8'h06) | (instruction == 8'h0B);
		assign read_cmd = (instruction == 8'h00) | (instruction == 8'h02);
		assign mem_cmd = (instruction == 8'h07) | (instruction == 8'h08) | (instruction == 8'h09) | (instruction == 8'h0A);

		always@(posedge sysClk) begin
			instruction_s <= instruction;
			data_s <= data;
		end

endmodule
