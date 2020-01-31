
module reg_read_register_table( input sysClk,
							 /* from the intr_data_buffer */
							 input [7:0]   reg_addr,
							 input [15:0] reg_data,
							 input        instr_valid_reg_stuff,

							 /*from the actual read registers*/
							 input [31:0]  read_vals,
							 input         reg_valid_read,

							 /*to the actual read registers*/
							 output error_reg_read,
							 output status_reg_read,

							 /*to the byte buffer that sends bytes to the SPI interface*/
							 output [7:0] byte_out,
							 output 			byte_out_valid
							 );

		assign status_reg_read = instr_valid_reg_stuff & (reg_addr = 8'h00);
		assign error_reg_read = instr_valid_reg_stuff & (reg_addr == 8'h04);


		wire[31:0] reg_buff;
		reg[31:0] reg_buff_r;

		assign reg_buff = reg_buff_r;

		wire done_reading_flag;
		reg done_reading_flag_r;

		assign done_reading_flag = done_reading_flag_r;

		wire [1:0] byte_counter;
		reg [1:0] byte_counter_r;

		assign byte_counter = byte_counter_r;


		wire begin_readout;

		always@(posedge sysClk) begin
			if(reg_valid_read) begin
					reg_buff_r <= read_vals;
					byte_counter_r <= 0;
					done_reading_flag_r <= 1;
			end
			else begin
				if(done_reading_flag) begin
					case (byte_counter)
							2'b00: byte_out <= reg_buff[7:0];
							2'b01: byte_out <= reg_buff[15:8];
							2'b10: byte_out <= reg_buff[23:16];
							2'b11: byte_out <= reg_buff[31:24];
							default: byte_out <= 8'h00;
					endcase
					byte_out_valid <= 1;
					done_reading_flag <= ~(byte_counter == 2'b11);
				end
				else begin
					byte_out_valid = 0;
				end
			end
		end


endmodule
