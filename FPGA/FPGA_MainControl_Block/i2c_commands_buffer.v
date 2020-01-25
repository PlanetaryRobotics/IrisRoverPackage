module i2c_commands_buffer(input [7:0]  saddr_from_cmdMap,
									input [15:0] sdata_from_cmdMap,
									input        rgb_from_cmdMap,
									input [1:0]  compression_from_cmdMap,
									input        trigger_from_cmdMap,
									input        cmdMap_input_valid,
									
									output [7:0] saddr_to_cam,
									output [15:0] sdata_to_cam,
									output       rgb_to_cam,
									output [1:0] compression_to_cam,
									output       trigger_to_cam,
									output       cmd_valid
									);

	
									
endmodule