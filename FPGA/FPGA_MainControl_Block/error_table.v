
module error_table(input         sysClk,
						 input [15:0]  in_index_of_error_reg,
						 input         in_camid_of_error_reg,
						 input         in_cam_timeout_error_reg,
						 input         in_img_cap_failure_error_reg,
						 input         in_cam_not_detected_error_reg,
						 input         in_write_failure_error_reg,
						 input         in_read_failure_error_reg,
						 input         in_erase_failure_error_reg,
									 
						 input         in_valid_error_reg,
						 
						 input         start_flush_error_reg,
			
			
						 output reg [22:0] out_error_reg,
						 output reg       out_valid_error_reg
						 );
						 
			wire [22:0] new_full_reg_buff;
			
			reg [22:0] err_table_row[7:0];
			reg err_table_counter[2:0];
			
			assign new_full_reg_buff = {in_index_of_error_reg[15:0], in_camid_of_error_reg, in_cam_timeout_error_reg, in_img_cap_failure_error_reg, in_cam_not_detected_error_reg, in_write_failure_error_reg, in_read_failure_error_reg, in_erase_failure_error_reg};   

			wire table_full_flag;
			reg [2:0] curr_counter_val;
			
			
			always @(posedge sysClk) begin
				if(in_valid_error_reg & (~(start_flush_error_reg)) begin
					err_table_row[err_table_counter] <= new_full_reg_buff;
					err_table_counter <= err_table_counter + 1;
				   curr_counter_val <= curr_counter_val + 1;	
				end
				if((err_table_counter == 3'b111) & (~(start_flush_error_reg)) begin
					table_full_flag = 1;
				end		
			end
			
			always @(posedge sysClk) begin
				if(start_flush_error_reg) begin
					out_error_reg <= err_table_row[err_table_counter];
					out_valid_error_reg = 1;
					
					if(table_full_flag) begin
						if(err_table_counter == curr_counter_val) begin
							out_valid_reg <= 0;
							table_full_flag <= 0;
						end
						else
							err_table_counter <= err_table_counter - 1;
						end
					end
					else begin
						if(err_table_counter == 3'b000) begin
							out_valid_reg <= 0;
						end
						else begin
							err_table_counter <= err_table_counter - 1;
						end
					end
				end
				else begin
					out_valid_error_reg <= 0;
				end
			end
			
									 
endmodule