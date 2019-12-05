`timescale 1ns / 1 ps


module spi_slave_tb;
	
		reg sysClk_tb; // FPGA main system clock
		reg SPICLK_tb; //clock line for SPI
		reg MOSI_tb; //data line from master to slave
		reg SS_tb; //Slave select line, initiates transaction
	   reg [7:0] txData_tb; //byte to push to master
		wire MISO_tb; //data line from slave to master
		wire [7:0] rxData_tb; //received byte from master
		wire rx_valid_tb; // indication when received an entire byte from master

		wire [47:0] value_from_master;
		
		assign value_from_master = 48'b000100101101000011001010000100101101000011001010;
		
		FPGA_SPI_Interface testslave(
		sysClk_tb, 
		SPICLK_tb, 
		MOSI_tb, 
		SS_tb, 
		txData_tb, 
		MISO_tb, 
		rxData_tb, 
		rx_valid_tb 
					
		);
		
		initial
			begin
				sysClk_tb = 1'b1;
				SPICLK_tb = 1'b1;
				MOSI_tb = 1'b0;
				SS_tb = 1'b1;
				txData_tb = 8'b0000000;
				
				#20;
				
				
				
				SS_tb = 0;
				txData_tb = 8'b00110001;
				
				#5;
				
				for(integer i = 0; i < 48; i = i + 1) begin
					SPICLK_tb = 0;
					sysClk_tb = 0;
					#5;
					SPICLK_tb = 1;
					sysClk_tb = 1;
					MOSI_tb = value_from_master[i];
				end
				
				#5;
				SS_tb = 1;
			end
		
endmodule