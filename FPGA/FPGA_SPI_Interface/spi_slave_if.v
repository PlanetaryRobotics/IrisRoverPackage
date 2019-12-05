/* 
 SPI slave interface
 
 - utilizes SPI mode 3
*/
 
`timescale 1ns / 1ps
`default_nettype none

module FPGA_SPI_Interface(

	input wire sysClk,      	// FPGA main system clock
	input wire SPICLK,        	// clock line for SPI
	input wire MOSI,        	// data line from master to slave
	input wire SS,          	// Slave select line, intitiates transaction
	input wire [7:0] txData,	// byte to push to master
	output wire MISO,       	// data line from slave to master
	output wire [7:0] rxData,	// recieved byte from master
	output wire rx_Valid 		// indication when recieved an entire byte from master
	
);  

	// Registers to keep track of SPI clock, SS, and MOSI line with 2 stage shift registers
	// use 3 stage registers from SS and SPICLK to determine when rising or falling for SPI metastability
	
	reg [2:0] SPICLK_r;  always @(posedge sysClk) SPICLK_r <= {SPICLK_r[1:0], SPICLK};
	reg [2:0] SS_r;    always @(posedge sysClk) SS_r   <= {SS_r[1:0], SS};
	reg [1:0] MOSI_r;  always @(posedge sysClk) MOSI_r <= {MOSI_r[0], MOSI};
	
	wire MOSI_sync    = MOSI_r[1];	// wire pulling from  MOSI shift register for metastability
	
	wire SS_active    = ~SS_r[1];					// wire pulling from SS register, inverted to be high when SS low
	wire SS_falling   = (SS_r[2:1] == 2'b10);	// wire to determine when SS falls, SPI transaction begins when falling
	
	wire SPICLK_rising  = (SPICLK_r[2:1] == 2'b01);	// wire to determine when to sample MOSI during SPI clock rising
	wire SPICLK_falling = (SPICLK_r[2:1] == 2'b10);	// wire to determine when to push to MISO line during SPI clock falling

	reg [7:0] cbuffer = 8'hxx;	// buffer to store incoming data in rxData circular buffer
	
	
	/* Byte state logic */

	reg [2:0] byte_state = 3'bxxx; // state logic used to keep track of bit count
	
	// always block to control the byte state
	//
	always @(posedge sysClk)
	
		if (SS_active)
			begin
				if (SS_falling)   	// if SS falls, begin transaction
				
					byte_state <= 3'd0;
					
				if (SPICLK_rising) // increment byte upon incoming bit
				
					byte_state <= byte_state + 3'd1;
			end
	
			
	/* data transfer logic */

	// [SPICLK_falling] cbuffer[7] pushed onto MISO line
	// [SPICLK_rising]  MOSI line data in shifted into buffer ({cbuffer[6:0], MISO}) on rxData
	
	assign rxData  = {cbuffer[6:0], MOSI_sync};       			// wire used as circular data line to store incoming data
	assign rx_Valid = (byte_state == 3'd7) && SPICLK_rising;	// byte is valid once 8 bits have been recieved during state

	reg MISO_r = 1'bx;								// register to store bit to be pushed to MISO line
	assign MISO = SS_active ? MISO_r : 1'bz;	// MISO line, set to z if SS high (inactive)
	
	// always block to control pushing and pulling data
	// - pull data from MOSI line on rising SPICLK
	// - push data to MISO line on falling SPICLK
	//
	always @(posedge sysClk)
	
		if(SS_active)
			begin
			
				// SAMPLING LINE, if state is not max, push current rxData line back into circular buffer.
				// from here the assign rx to {cbuffer[6:0], MOSI_sync} takes the bottom bits of the buffer
				// and the latest from the MOSI line, and pops off the oldest buffer data
				// essentially sampling the MOSI data and storing it on the rising edge (SPI mode 3)
				
				if(SPICLK_rising)         // INPUT on rising SPI clock edge
				
					if(byte_state != 3'd7) 
					
						cbuffer <= rxData;
							
				// on falling edge of SPICLK, push bits from cbuffer that we just set from rx in a recursive fashion
				// onto MISO_r, or sample tx and push that into MISO_r and dump remaining bits into cbuffer
				// as circular buffer will push the rest of the bits out of buffer in MOSI_sync sampling routine
						
				if( SPICLK_falling)         			// OUTPUT on falling SPI clock edge
				
					if (byte_state == 3'b000)
						begin 
							cbuffer <= txData;       //   remaining bits are sent from cbuffer
							MISO_r <= txData[7];    //   start by sending the MSb
						end				
						
					else
						MISO_r <= cbuffer[7];  //   send next bit, will be pushed from circular buffer after
						
			end						
endmodule
