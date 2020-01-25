module mem_register_table(input        sysClk, //system clock
							 /* from the intr_data_buffer */
								  input [7:0]  reg_addr, 
								  input [16:0] reg_data,
								  input        reg_input_valid,
								
							 /*	from mem interface block */
								  input [31:0] jpg_size, 
								  input        jpg_size_valid,
								  
							 /*   from num_img_stored_reg */	  
								  input [31:0] num_both_img, 
								  input        num_both_img_valid,
								  
							 
					       /* to num_img_stored_reg */		 
								  output 		 read_num_img,
								  
							 /*  to mem interface block  */	  
								  output 		 read_img_size,
								  output [15:0] img_index,
								  
								  output [7:0]  byte_out,
								  output        byte_out_valid
								  );
	
		
	
endmodule
								  