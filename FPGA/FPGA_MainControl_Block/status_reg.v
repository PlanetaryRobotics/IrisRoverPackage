module status_reg(input 		in_proc_img_1_status_reg,
						input 		in_proc_img_2_status_reg,
						input 		in_erase_img_proc_status_reg,
						input 		in_error_bit_status_reg,
						input 		in_booting_status_reg,
						
						input 		in_valid_status_reg,
						
						input			start_flush_status_reg,
						
						
						output[7:0] reg out_status_reg,
						output      out_valid_status_reg					
						);

		wire[4:0] stat_buff;
		
		assign stat_buff = {in_booting_status_reg, in_error_bit_status_reg, in_erase_img_proc_status_reg, in_proc_img_2_status_reg, in_proc_img_1_status_reg);
		
		always@(*) begin
			if(start_flush_status_reg) begin
				out_status_reg = stat_buff;
				out_valid_status_reg = 1;
			end
			else begin
				out_valid_status_reg = 0;
			end
		
		end
endmodule