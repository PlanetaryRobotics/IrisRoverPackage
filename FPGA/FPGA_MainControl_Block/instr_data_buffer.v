module instr_data_buffer(input sysClk,
								 input [7:0] byte, // byte from spi interface
								 input rx_valid,  // is the byte valid?
								 
								 output reg [7:0]   instruction,
								 output reg [127:0] data,
								 output reg         valid_buffer_for_camwrite,
								 output reg         valid_buffer_for_read,
								 output reg         valid_buffer_for_mem
								 );
		
		
		wire camwrite_cmd;
		wire read_cmd;
		wire mem_cmd;
		
		wire output_complete;
		
		reg [4:0] byte_counter;
		
		reg [135:0] next_byte_buffer;
		wire[135:0] byte_buffer;
		
		assign byte_buffer = next_byte_buffer;
		
		assign output_complete = valid_buffer_for_camwrite | valid_buffer_for_read | valid_buffer_for_mem;
		
		
		assign camwrite_cmd = (byte_buffer[135:128] == 2'h01) | (byte_buffer[135:128] == 2'h02) | (byte_buffer[135:128] == 2'h03) | (byte_buffer[135:128] == 2'h05) | (byte_buffer[135:128] == 2'h06);
		assign read_cmd = (byte_buffer[135:128] == 2'h00) | (byte_buffer[135:128] == 2'h04);
		assign mem_cmd = (byte_buffer[135:128] == 2'h07) | (byte_buffer[135:128] == 2'h08) | (byte_buffer[135:128] == 2'h09) | (byte_buffer[135:128] == 2'0A);
		
		
		
		
	   always @(posedge sysClk) begin
			if(rx_valid & (~output_complete)) begin
				next_byte_buffer <= ((byte_buffer | byte) << 8);
				byte_counter <= byte_counter + 1;
			end
		end
		
		always @(posedge sysClk) begin
			if(byte_counter == 5'10001) begin
				instruction <= byte_buffer[135:128];
				data <= byte_buffer[127:0];
				valid_buffer_for_camwrite <= camwrite_cmd;
				valid_buffer_for_read <= read_cmd;
				valid_buffer_for_mem <= mem_cmd;
				next_byte_buffer <= 0;
				byte_counter <= 0;
			end
			else begin
				valid_buffer_for_camwrite <= 0;
				valid_buffer_for_read <= 0;
				valid_buffer_for_mem <= 0;
			end
		end
endmodule
								 