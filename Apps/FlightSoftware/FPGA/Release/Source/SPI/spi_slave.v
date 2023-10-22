////////////////////////////////////////////////////////////////////////////////
////                                                                        ////
//// Project Name: SPI (Verilog)                                            ////
////                                                                        ////
//// Module Name: spi_slave                                                ////
////                                                                        ////
////                                                                        ////
////  This file is part of the Ethernet IP core project                     ////
////  http://opencores.com/project,spi_verilog_master_slave                 ////
////                                                                        ////
////  Author(s):                                                            ////
////      Santhosh G (santhg@opencores.org)                                 ////
////                                                                        ////
////  Refer to Readme.txt for more information                              ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////                                                                        ////
//// Copyright (C) 2014, 2015 Authors                                       ////
////                                                                        ////
//// This source file may be used and distributed without                   ////
//// restriction provided that this copyright statement is not              ////
//// removed from the file and that any derivative work contains            ////
//// the original copyright notice and the associated disclaimer.           ////
////                                                                        ////
//// This source file is free software; you can redistribute it             ////
//// and/or modify it under the terms of the GNU Lesser General             ////
//// Public License as published by the Free Software Foundation;           ////
//// either version 2.1 of the License, or (at your option) any             ////
//// later version.                                                         ////
////                                                                        ////
//// This source is distributed in the hope that it will be                 ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied             ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR                ////
//// PURPOSE.  See the GNU Lesser General Public License for more           ////
//// details.                                                               ////
////                                                                        ////
//// You should have received a copy of the GNU Lesser General              ////
//// Public License along with this source; if not, download it             ////
//// from http://www.opencores.org/lgpl.shtml                               ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
/* SPI MODE 3
		CHANGE DATA (MISO) @ NEGEDGE SCK
		read data (MOSI) @posedge SCK
*/
module spi_slave (reset,ten,tdata,mlb,SS,SCK,MOSI, MISO,done,rdata);
  input reset,SS,SCK,MOSI,ten,mlb;
  input [7:0] tdata;
  output MISO;           //slave out   master in
  output reg done;
  output reg [7:0] rdata;

  reg [7:0] treg,rreg;
  reg [3:0] nb;
  wire sout;

  assign sout=mlb?treg[7]:treg[0];
  assign MISO=( (!SS)&&ten )?sout:1'bz; //if 1=> send data  else TRI-STATE MISO


//read from  MISO
always @(posedge SCK or negedge reset)
  begin
    if (reset==0)
		begin rreg = 8'h00;  rdata = 8'h00; done = 0; nb = 0; end   //
	else if (!SS) begin
			if(mlb==0)  //LSB first, in@msb -> right shift
				begin rreg ={MOSI,rreg[7:1]}; end
			else     //MSB first, in@lsb -> left shift
				begin rreg ={rreg[6:0],MOSI}; end
		//increment bit count
			nb=nb+1;
			if(nb!=8) done=0;
			else  begin rdata=rreg; done=1; nb=0; end
		end	 //if(!SS)_END  if(nb==8)
  end

//send to  MISO
always @(negedge SCK or negedge reset)
  begin
	if (reset==0)
		begin treg = 8'hFF; end
	else begin
		if(!SS) begin
			if(nb==0) treg=tdata;
			else begin
			   if(mlb==0)  //LSB first, out=lsb -> right shift
					begin treg = {1'b1,treg[7:1]}; end
			   else     //MSB first, out=msb -> left shift
					begin treg = {treg[6:0],1'b1}; end
			end
		end //!SS
	 end //reset
  end //always

endmodule

/*
			if(mlb==0)  //LSB first, out=lsb -> right shift
					begin treg = {treg[7],treg[7:1]}; end
			else     //MSB first, out=msb -> left shift
					begin treg = {treg[6:0],treg[0]}; end
*/


/*
force -freeze sim:/SPI_slave/SCK 0 0, 1 {25 ns} -r 50 -can 410
run 405ns
noforce sim:/SPI_slave/SCK
force -freeze sim:/SPI_slave/SCK 1 0
*/
