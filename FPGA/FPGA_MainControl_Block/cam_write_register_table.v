
/*
		name(of cmd to cam/cam-int)| addr(s)       | args for cam                    | args for interface                                     | instructions to camera
		_______________________________________________________________________________________________________________________________________________________________

		status register            | 0x00          | none                            | proc image cam 1/2, erase img proc, error bit, booting |  none
		control reg						| 0x01			 | none									  | camera id, index                                       |  
		config cam1 & 2            | 0x02 and 0x03 | exposure val, shutter speed     | RGB, compression qf                                    | 
		error register             | 0x04          | none                            | read reg, clear reg                                    |
		crop reg cam 1 & 2         | 0x05 and 0x06 | upper x, lowerx, upper, lower y | none                                                   |
				
*/
module cam_val_converter(input[2:0] val_type,
								 input[2:0] step,
								 input[31:0] val_in,
								 
								 output[15:0] val_out
								 );
		
		
endmodule


module cam_i2c_command_map( input        sysClk,
						          input[7:0]   reg_addr,
						          input[127:0] reg_data,
									 input        valid_input,
									 input		  instr_counter,
									 
									 output [7:0]  cam_i2c_addr,
									 output [15:0] cam_i2c_data,
									 output        cam_i2c_output_valid,
									 output        all_instr_out

						 );

endmodule


module cam_write_register_table(input           sysClk, //clock
										  /*from instruction buffer */			
										  input [7:0]     reg_addr,
										  input [127:0]   reg_data,										  
										  input    		   intr_valid_input,
										  
										  /*from slave copy buffer */
										  input [15:0]   return_slave_copy,
										  input           slave_copy_valid, 
											
											
										  /* cam i2c stuff */	
										  output [7:0]  cam_i2c_addr,
										  output [15:0] cam_i2c_data,										  
										  /* cam interface config stuff */
										  output [1:0]  compression,
										  output        RGB,
										  
										  /*all cam stuff*/
										  output        cam_id,
										  output        output_valid
										  );

										  
		
endmodule
										  