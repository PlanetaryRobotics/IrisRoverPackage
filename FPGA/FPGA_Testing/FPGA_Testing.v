`timescale 1ns/1ns


module FPGA_Testing(input reset, input clk, output reg LED);

	reg [32:0] counter;
	reg state;


	always @ (posedge clk) begin
		if(reset == 0) begin
			counter <= 0;
			state <= 0;
		end
		else begin	
			counter <= counter + 1;
			state <= counter[26];
		end
		
	end

	always @* begin
	 LED =  state;
	end
	

endmodule