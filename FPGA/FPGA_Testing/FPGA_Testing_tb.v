`timescale 1ns/1ns


module FPGA_Testing_tb;

	 reg clk;
	 wire LED;
	 reg reset;
	 
	 FPGA_Testing test(
					  reset, 
					  clk, 
					  LED
					  );
	 
	 
	initial 	
	begin
		clk = 0;
		reset = 1;
		#10
		reset = 0;
	end
	
	always
	begin
		#5 clk = !clk;
	end
	
	
endmodule