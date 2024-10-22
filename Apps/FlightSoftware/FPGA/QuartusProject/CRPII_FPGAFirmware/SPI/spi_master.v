////////////////////////////////////////////////////////////////////////////////
////                                                                        ////
//// Project Name: SPI (Verilog)                                            ////
////                                                                        ////
//// Module Name: spi_master                                                ////
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
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SPI MODE 3
		CHANGE DATA (MOSI) @ NEGEDGE SCK
    READ DATA (MISO) @ POSEDGE SCK

 RSTB-active low asyn reset, CLK-clock, T_RB=0-rx  1-TX, mlb=0-LSB 1st 1-msb 1st
 START=1- starts data transmission clk_to_sck_divisor 0=clk/4 1=/8   2=/16  3=/32
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
module spi_master(reset,clk,mlb,start_SPI_transfer,transmit_data,clk_to_sck_divisor,MISO, SS,SCK,MOSI,done,received_data, multiple_byte_transfer);

  // System Inputs
  input reset;
  input clk;

  // SPI Interface
  output reg SCK;  // Serial Clock
  output reg SS;   // Slave Select
  input      MISO; // Master In Slave Out
  output reg MOSI; // Master Out Slave In

  // Control IO
  input mlb;
  input start_SPI_transfer; // Start sending data
  input [1:0] clk_to_sck_divisor;  // Divisor for generating SCK
  output reg done;  // Done sending data
  input multiple_byte_transfer; // Flag for allowing consecutive bytes to be sent

  // Data Buffers
  input [7:0] transmit_data;  //transmit data
  output reg [7:0] received_data; //received data


// FSM States
parameter idle            = 2'b00;
parameter send            = 2'b10;
parameter resetting   = 2'b11;
reg [1:0] current_state, next_state;

// FSM registers
reg [7:0] treg;
reg [7:0] rreg;
reg [3:0] nbit;
reg [4:0] mid;
reg [4:0] cnt;
reg shift;
reg clr;

reg hold_done;

// FSM IO
always @(start_SPI_transfer or current_state or nbit or clk_to_sck_divisor or rreg) begin

  next_state=current_state;
  clr=0;
  shift=0;//SS=0;

  // State transition definitions
  case(current_state)

    // IDLE state
    idle: begin
      if(start_SPI_transfer==1) begin
        case (clk_to_sck_divisor)
          2'b00: mid=2;
          2'b01: mid=4;
          2'b10: mid=8;
          2'b11: mid=16;
        endcase
        shift=1;
        done=1'b0;
        next_state=send;

      end
    end // IDLE state

    // SEND state
    send: begin
      SS=0;
      if(nbit!=8) begin
        shift=1;

        if(multiple_byte_transfer) begin
          if (nbit > 0) begin
            done = 1'b0;
          end
        end
        else begin
          done = 1'b0;
        end

      end
      else begin
        if(multiple_byte_transfer == 1) begin
          received_data=rreg;
          done=1'b1;
          // hold_done = 1'b1;
          clr=1;
          next_state = send;
        end
        else begin
          received_data=rreg;
          done=1'b1;
          next_state=resetting;
        end
      end
    end // SEND state

    // RESET state
    resetting: begin
      shift=0;
      SS=1;
      clr=1;
      next_state = idle;
      done = 1'b0;
    end // RESET state

    // Default state
    default: next_state=resetting;

  endcase
end // always


// State transition @ NEGEDGE clk
always@(negedge clk or negedge reset) begin
  if(reset==0)
    current_state<=resetting; // Finish state handles buffer resets
  else
    current_state<=next_state;
end


// Setup on falling edge (shift MOSI) sample on rising edge (read MISO)

// Clock divider for generating SCK
always@(negedge clk or posedge clr) begin
  if(clr == 1) begin
    cnt = 0;
    SCK = 1;
  end
  else begin
    if(shift == 1) begin
      cnt= cnt+1;
      if(cnt == mid) begin
        SCK = ~SCK;
        cnt = 0;
      end //mid
    end //shift
  end //rst
end //always


// Sample MISO @ rising edge of SCK
always@(posedge SCK or posedge clr) begin // or negedge reset

  // Reset
  if(clr == 1) begin
    nbit = 0;
    rreg = 8'hFF;
  end

  // Shift received_data_buffer and place MISO bit accordingly
  else begin

    // Right shift: data's LSB received first, MISO placed at MSB
    if(mlb == 0) begin
      rreg = {MISO, rreg[7:1]};
    end

    // Left shift: data's MSB recieved first, MISO placed at LSB
    else begin
      rreg = {rreg[6:0], MISO};
    end

    // Count received bits
    nbit = nbit+1;

  end // Reset
end // always


// Send MOSI @ falling edge of SCK
always@(negedge SCK) begin

  // Reset
  // if(clr == 1) begin
  //   // treg = 8'hFF;
  //   MOSI = 1;
  // end
  //
  // // Shift transmission_data_buffer and place MOSI bit accordingly
  // else begin

    // Load data into TREG
    if(nbit == 0) begin
      treg = transmit_data;
      MOSI = mlb ? treg[7]:treg[0];
    end //nbit_if

    // Shift TREG and set MOSI
    else begin

      // Right shift: Send data's MSB first to MOSI
      if(mlb == 0) begin
        treg = {1'b1, treg[7:1]}; // Shift TREG
        MOSI = treg[0];
      end
      // Left shift: Send data's MSB first to MOSI
      else begin
        treg = {treg[6:0], 1'b1}; // Shift TREG
        MOSI = treg[7];
      end
    end

 // end // Reset
end // always


endmodule
