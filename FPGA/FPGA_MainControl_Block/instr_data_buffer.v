module instr_data_buffer(input sysClk,
								 input [7:0] byte_in, // byte from spi interface
								 input rx_valid,  // is the byte valid?

								 output  [7:0]   instruction,
								 output  [63:0]  data,
								 output          valid_buffer_for_camwrite,
								 output          valid_buffer_for_read,
								 output          valid_buffer_for_mem
								 );

		
		reg [7:0]   instruction_s;
	   reg [63:0]  data_s;
		reg         valid_buffer_for_camwrite_s;
	   reg         valid_buffer_for_read_s;
		reg         valid_buffer_for_mem_s;
								 
		assign instruction = instruction_s;
		assign data = data_s;
		assign valid_buffer_for_camwrite = valid_buffer_for_camwrite_s;
		assign valid_buffer_for_read = valid_buffer_for_read_s;
		assign valid_buffer_for_mem = valid_buffer_for_mem_s;
		
		wire camwrite_cmd;
		wire read_cmd;
		wire mem_cmd;

		wire output_complete;

		wire[3:0] byte_counter;
		reg [3:0] byte_counter_s;

		assign byte_counter = byte_counter_s;
		
		
		
		reg [71:0] next_byte_buffer;
		wire[71:0] byte_buffer;

		assign byte_buffer = next_byte_buffer;

		wire[71:0] completed_buffer;
		reg[71:0]  completed_buffer_s;
		assign completed_buffer = completed_buffer_s;
		
	
		reg running_s;
		wire running;
		
		assign running = running_s;
		
		initial begin
			byte_counter_s = 0;
			next_byte_buffer = 0;
			instruction_s <= 0;
			data_s <= 0;
			running_s <= 0; 
			completed_buffer_s <= 0;
		end
		
		assign output_complete = valid_buffer_for_camwrite_s | valid_buffer_for_read_s | valid_buffer_for_mem_s;


		assign camwrite_cmd = (completed_buffer[71:64] == 8'h01) | (completed_buffer[71:64] == 8'h03) | (completed_buffer[71:64] == 8'h04) | (completed_buffer[71:64] == 8'h05) | (completed_buffer[71:64] == 8'h06) | (completed_buffer[71:64] == 8'h0B);
		assign read_cmd = (completed_buffer[71:64] == 8'h00) | (completed_buffer[71:64] == 8'h02);
		assign mem_cmd = (completed_buffer[71:64] == 8'h07) | (completed_buffer[71:64] == 8'h08) | (completed_buffer[71:64] == 8'h09) | (completed_buffer[71:64] == 8'h0A);

		
		wire[7:0] shift_amount;
		assign shift_amount = ((4'b1000 - byte_counter)<<3);
		
	   always @(posedge sysClk) begin
			if(rx_valid & (~(output_complete))) begin
				next_byte_buffer <=  (byte_in << (shift_amount));
				completed_buffer_s <= (completed_buffer | byte_buffer);
				byte_counter_s <= byte_counter + 1;	
				running_s <= 1;
			end
			if((running &(~(rx_valid)))) begin
				instruction_s <= completed_buffer[71:64];
				data_s <= completed_buffer[63:0];

				valid_buffer_for_camwrite_s <= camwrite_cmd;
				valid_buffer_for_read_s <= read_cmd;
				valid_buffer_for_mem_s <= mem_cmd;
				next_byte_buffer <= 0;
				byte_counter_s <= 0;
				running_s <= 0;
			end
			else begin
				valid_buffer_for_camwrite_s <= 0;
				valid_buffer_for_read_s <= 0;
				valid_buffer_for_mem_s <= 0;
			end
			
		end


endmodule
