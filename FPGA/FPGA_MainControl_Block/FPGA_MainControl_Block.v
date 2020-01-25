module FPGA_MainControl_Block(input [7:0] byte_from_spi, 
								  input [15:0] bytes_from_cam, 
								  input[7:0] byte_from_mem, 
								  input spi_input_valid, 
								  input cam_input_valid,
								  input mem_input_valid,
								  input sysClk,
								  
								  output [7:0] byte_to_spi,
								  output[15:0] bytes_to_cam,
								  output[7:0] byte_to_mem,
								  output spi_start,
								  output cam_start,
								  output mem_start
								  );
			
			
					
			
endmodule
								  