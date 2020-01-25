
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
							 output [7:0] byte_out
							 );
		
		assign status_reg_read = instr_valid_reg_stuff & (reg_addr = 8'h00);
		assign error_reg_read = instr_valid_reg_stuff & (reg_addr == 8'h04); 

		
		wire[31:0] reg_buff;
		wire begin_readout;
		
		always@(posedge sysClk) begin
			if(reg_valid_read) begin
				
			end
		end
							 
							
endmodule							 
							 