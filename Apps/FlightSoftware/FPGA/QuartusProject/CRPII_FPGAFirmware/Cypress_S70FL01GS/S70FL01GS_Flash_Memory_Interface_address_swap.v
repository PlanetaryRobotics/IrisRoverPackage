// SPI Flash Interface - Cypress S70FL01GS
//  - Dual-Died Cypress S25FL512S
//  - Normal Data Rate (Single Data Rate - SDR) Operation
//  - Hardware Reset Active Low

module S70FL01GS_Flash_Memory_Interface_address_swap(

  // System Inputs
  input reset,
  input clk,

  // To Flash Memory (S70FL01GS) - SPI Slave
  output     flash_reset,
  output     flash_SCK,
  output     flash_CS1,
  output     flash_CS2,
  output     flash_SI,
  input      flash_SO,
  output wire flash_WP,
  output wire flash_HOLD,
  output reg flash_mem_interface_done, // Flash memory controller is idle

  output wire flash_mem_interface_spi_cycle_done,
  output reg flash_mem_interface_output_valid,

  input READ_ID,
  input RDSR1,
  input WREN,
  input READ3,

  input PP3,
  input PP4,

  input [31:0] address,
  input [31:0] num_bytes,
  // How many PP3 bytes have been written in the current PP3 transaction:
  output [9:0] PP3_send_bytes_counter; // (shouldn't ever exceed 512B <- max size of a page for PP3. Added extra MSB for safety.)

  input [7:0] input_data,
  output reg [7:0] output_data,


  // Debug
  output debug_out
);



// For reset state
reg spi_master_is_reset = 1'b0;

// Chip select from spi master
//  Here we would have to mux according to the desired address
//  Instead, we are only storing a single picture, thus hard coding chip select
// wire SS_reg;
// assign flash_CS1 = SS_reg;
assign flash_CS2 = 1'b1;

// SPI Master single/multi cycle flag
reg spi_master_multi_cycle_flag = 1'b0;

reg [7:0] spi_master_command = 8'h00;
wire [7:0] spi_master_response;
reg spi_master_enable = 1'b0;
wire spi_master_busy;
wire spi_master_done;
reg spi_master_reset = 1'b0;







// Not used - drive high
assign flash_WP = 1'b1;
assign flash_HOLD = 1'b1;
assign flash_reset = 1'b1;

assign debug_out = spi_master_done;



// Commands

reg [7:0] PP3_command       = 8'h02; // Page Program (3 byte addressing)
reg [7:0] READ3_command     = 8'h03; // Read Flash (3 byte addressing)

reg [7:0] RDSR1_command     = 8'h05; // Read Status Register 1 - WREL located here
reg [7:0] WREN_command      = 8'h06; // Write Enable - required before every page program

reg [7:0] PP4_command       = 8'h12; // Page Program (4 byte addressing)
reg [7:0] READ4_command     = 8'h13; // Read Flash (4 byte addressing)

reg [7:0] BRRD_command      = 8'h16; // Bank Register Read
reg [7:0] BRWR_command      = 8'h17; // Bank Register Write

reg [7:0] READ_ID_command   = 8'h90; // Read Device Manufacturer ID and Device ID


// Bank Address Register Setting
//  Set Extended Address Enable to high, BAR[7] - Enables 4 byte memory addressing
reg [7:0] BAR_setting = 8'b10000000;








// FSM States
reg [4:0] current_state, next_state;

parameter reset_state                = 5'd0;
parameter idle                       = 5'd1;

parameter READ_ID_send_instruction   = 5'd2;
parameter READ_ID_send_address       = 5'd3;
parameter READ_ID_get_bytes          = 5'd4;

parameter RDSR1_send_instruction     = 5'd5;
parameter RDSR1_get_byte             = 5'd6;

parameter WREN_send_instruction      = 5'd7;

parameter READ3_send_instruction     = 5'd8;
parameter READ3_send_address         = 5'd9;
parameter READ3_get_bytes            = 5'd10;

parameter PP3_send_instruction       = 5'd11;
parameter PP3_send_address           = 5'd12;
parameter PP3_send_bytes             = 5'd13;

parameter PP4_send_instruction       = 5'd14;
parameter PP4_send_address           = 5'd15;
parameter PP4_send_bytes             = 5'd16;




//##############################################
// READ_ID
//##############################################

reg READ_ID_instruction_was_sent = 1'b0;

reg reset_READ_ID_send_address_counter = 1'b0;
reg [2:0] READ_ID_send_address_counter = 3'd0;

reg reset_READ_ID_get_bytes_counter = 1'b0;
reg [2:0] READ_ID_get_bytes_counter = 3'd0;

reg [7:0] READ_ID_response = 8'd0;

// READ_ID_instruction_was_sent
always@(posedge spi_master_done) begin
  // Indicate the first spi_master_done
  if(current_state == READ_ID_send_instruction)
    READ_ID_instruction_was_sent = 1'b1;
  else
    READ_ID_instruction_was_sent = 1'b0;
end

// READ_ID_send_address_counter
always@(posedge spi_master_done or posedge reset_READ_ID_send_address_counter) begin
  // Reset counter
  if (reset_READ_ID_send_address_counter) begin
    READ_ID_send_address_counter = 0;
  end
  // Increment counter in correct state
  else if(current_state == READ_ID_send_address) begin
    READ_ID_send_address_counter = READ_ID_send_address_counter + 1;
  end
end

// READ_ID_get_bytes_counter
always@(posedge spi_master_done or posedge reset_READ_ID_get_bytes_counter) begin
  // Reset counter
  if (reset_READ_ID_get_bytes_counter) begin
    READ_ID_get_bytes_counter = 0;
  end
  // Increment counter in correct state
  else if(current_state == READ_ID_get_bytes) begin
    READ_ID_response = spi_master_response;
    READ_ID_get_bytes_counter = READ_ID_get_bytes_counter + 1;
  end
end


//##############################################
// RDSR1
//##############################################

reg RDSR1_instruction_was_sent = 1'b0;

reg RDSR1_response_was_received = 1'b0;

reg [7:0] RDSR1_response = 8'd0;

// RDSR1_instruction_was_sent
always@(posedge spi_master_done) begin
  // Indicate the first spi_master_done
  if(current_state == RDSR1_send_instruction)
    RDSR1_instruction_was_sent = 1'b1;
  else
    RDSR1_instruction_was_sent = 1'b0;
end

// RDSR1_response_was_received
always@(posedge spi_master_done) begin
  // Indicate spi_master_done
  if(current_state == RDSR1_get_byte) begin
    RDSR1_response_was_received = 1'b1;

    // Output data reflects command response
    RDSR1_response = spi_master_response;
  end
  else
    RDSR1_response_was_received = 1'b0;
end


//##############################################
// WREN
//##############################################

reg WREN_instruction_was_sent = 1'b0;

// WREN_instruction_was_sent
always@(posedge spi_master_done) begin
  // Indicate the first spi_master_done
  if(current_state == WREN_send_instruction)
    WREN_instruction_was_sent = 1'b1;
  else
    WREN_instruction_was_sent = 1'b0;
end


//##############################################
// READ3
//##############################################

reg READ3_instruction_was_sent = 1'b0;

reg reset_READ3_send_address_counter = 1'b0;
reg [2:0] READ3_send_address_counter = 3'd0;

reg reset_READ3_get_bytes_counter = 1'b0;
reg [31:0] READ3_get_bytes_counter = 32'd0;

reg [7:0] READ3_response = 8'd0;

// READ3_instruction_was_sent
always@(posedge spi_master_done) begin
  // Indicate the first spi_master_done
  if(current_state == READ3_send_instruction)
    READ3_instruction_was_sent = 1'b1;
  else
    READ3_instruction_was_sent = 1'b0;
end

// READ3_send_address_counter
always@(posedge spi_master_done or posedge reset_READ3_send_address_counter) begin
  // Reset counter
  if (reset_READ3_send_address_counter) begin
    READ3_send_address_counter = 0;
  end
  // Increment counter in correct state
  else if(current_state == READ3_send_address) begin
    READ3_send_address_counter = READ3_send_address_counter + 1;
  end
end

// READ3_get_bytes_counter
always@(posedge spi_master_done or posedge reset_READ3_get_bytes_counter) begin
  // Reset counter
  if (reset_READ3_get_bytes_counter) begin
    READ3_get_bytes_counter = 0;
  end
  // Increment counter in correct state
  else if(current_state == READ3_get_bytes) begin
    READ3_response = spi_master_response;
    READ3_get_bytes_counter = READ3_get_bytes_counter + 1;
  end
end


//##############################################
// PP3
//##############################################

reg PP3_instruction_was_sent = 1'b0;

reg reset_PP3_send_address_counter = 1'b0;
reg [2:0] PP3_send_address_counter = 3'd0;

reg reset_PP3_send_bytes_counter = 1'b0;
initial begin
  PP3_send_bytes_counter = 10'd0;
end

// PP3_instruction_was_sent
always@(posedge spi_master_done) begin
  // Indicate the first spi_master_done
  if(current_state == PP3_send_instruction)
    PP3_instruction_was_sent = 1'b1;
  else
    PP3_instruction_was_sent = 1'b0;
end

// PP3_send_address_counter
always@(posedge spi_master_done or posedge reset_PP3_send_address_counter) begin
  // Reset counter
  if (reset_PP3_send_address_counter) begin
    PP3_send_address_counter = 0;
  end
  // Increment counter in correct state
  else if(current_state == PP3_send_address) begin
    PP3_send_address_counter = PP3_send_address_counter + 1;
  end
end

// PP3_send_bytes_counter
always@(posedge spi_master_done or posedge reset_PP3_send_bytes_counter) begin
  // Reset counter
  if (reset_PP3_send_bytes_counter) begin
    PP3_send_bytes_counter = 10'd0;
  end
  // Increment counter in correct state
  else if(current_state == PP3_send_bytes) begin
    PP3_send_bytes_counter = PP3_send_bytes_counter + 10'd1;
  end
end



//##############################################
// PP4
//##############################################

reg PP4_instruction_was_sent = 1'b0;

reg reset_PP4_send_address_counter = 1'b0;
reg [2:0] PP4_send_address_counter = 3'd0;

reg reset_PP4_send_bytes_counter = 1'b0;
reg [31:0] PP4_send_bytes_counter = 32'd0;

reg [7:0] PP4_write_data= 8'd0;

// PP4_instruction_was_sent
always@(posedge spi_master_done) begin
  // Indicate the first spi_master_done
  if(current_state == PP4_send_instruction)
    PP4_instruction_was_sent = 1'b1;
  else
    PP4_instruction_was_sent = 1'b0;
end

// PP4_send_address_counter
always@(posedge spi_master_done or posedge reset_PP4_send_address_counter) begin
  // Reset counter
  if (reset_PP4_send_address_counter) begin
    PP4_send_address_counter = 0;
  end
  // Increment counter in correct state
  else if(current_state == PP4_send_address) begin
    PP4_send_address_counter = PP4_send_address_counter + 1;
  end
end

// PP4_send_bytes_counter
always@(posedge spi_master_done or posedge reset_PP4_send_bytes_counter) begin
  // Reset counter
  if (reset_PP4_send_bytes_counter) begin
    PP4_send_bytes_counter = 0;
  end
  // Increment counter in correct state
  else if(current_state == PP4_send_bytes) begin
    PP4_write_data = input_data;
    PP4_send_bytes_counter = PP4_send_bytes_counter + 1;
  end
end




// State Machine
always @(posedge clk) begin


  next_state=current_state;

  // State transition definitions
  case(current_state)

    // RESET state
    reset_state: begin

      // Move to idle when finished, deactivate SPI master reset, deassert reset flag
      if(spi_master_is_reset == 1) begin
        next_state = idle;
        spi_master_reset = 1'b1;
        spi_master_is_reset = 1'b0;
      end

      // Reset SPI master
      else begin
        if(!spi_master_is_reset) begin
          spi_master_reset = 1'b0;
          spi_master_is_reset = 1'b1;
        end
      end

    end


    // IDLE state
    idle: begin

      // Move to reset state
      if(!reset)
        next_state = reset_state;

      // Idle state
      else begin
        // Ensure done signal remains low or finishes a pulse upon transitioning to idle state
        flash_mem_interface_done = 1'b0;

        // All flags/counter resets for states that transition to idle state are deactivated
        reset_READ_ID_get_bytes_counter = 1'b0;
        reset_READ3_get_bytes_counter = 1'b0;
        reset_PP3_send_bytes_counter = 1'b0;
        reset_PP4_send_bytes_counter = 1'b0;



        // Function flags determine transitions from idle state
        if(READ_ID) begin
          next_state = READ_ID_send_instruction;
        end

        else if(RDSR1) begin
          next_state = RDSR1_send_instruction;
        end

        else if(WREN) begin
          next_state = WREN_send_instruction;
        end

        else if(READ3) begin
          next_state = READ3_send_instruction;
        end

        else if(PP3) begin
          next_state = PP3_send_instruction;
        end

        else if(PP4) begin
          next_state = PP4_send_instruction;
        end

      end

    end



    //##############################################
    // READ_ID
    //##############################################

    // Send Instruction
    READ_ID_send_instruction: begin

      // Flash interface is busy
      flash_mem_interface_done = 1'b0;

      // Set instruction and enable SPI master for multi-cycle transfer
      spi_master_command = READ_ID_command;
      spi_master_enable = 1'b1;
      spi_master_multi_cycle_flag = 1'b1;

      // Move to next state when done
      if (READ_ID_instruction_was_sent) begin
        next_state = READ_ID_send_address;
      end

    end

    // Send address
    // READ_ID command requires address of 24'h000000
    READ_ID_send_address: begin

      // Move to next state when done counting and reset counter
      if(READ_ID_send_address_counter == 3) begin
        next_state = READ_ID_get_bytes;
        reset_READ_ID_send_address_counter = 1'b1;
      end

      // Set address bytes
      else begin
        spi_master_command = 8'h00;
      end

    end

    // Get bytes
    READ_ID_get_bytes: begin

      // Deassert previous state's counter reset
      reset_READ_ID_send_address_counter = 1'b0;

      // Blank SPI master transmission data
      spi_master_command = 8'h00;

      // Output data reflects command response
      output_data = READ_ID_response;

      // Move to next state when done
      if(flash_mem_interface_done) begin
        next_state = idle;
      end

      // Last byte received, disable SPI master, reset counter, and start done pulse
      else if(READ_ID_get_bytes_counter == 2) begin
        flash_mem_interface_done = 1'b1;
        reset_READ_ID_get_bytes_counter = 1;
        spi_master_enable = 1'b0;
      end

      // Second to last byte, inform SPI master of last SPI cycle
      else if(READ_ID_get_bytes_counter == 1) begin
        spi_master_multi_cycle_flag = 1'b0;
      end

    end


    //##############################################
    // RDSR1
    //##############################################

    // Send Instruction
    RDSR1_send_instruction: begin

      // Flash interface is busy
      flash_mem_interface_done = 1'b0;

      // Set instruction and enable SPI master for multi-cycle transfer
      spi_master_command = RDSR1_command;
      spi_master_enable = 1'b1;
      spi_master_multi_cycle_flag = 1'b1;

      // Move to next state when done
      //  Only expecting one more byte, also inform SPI master of last SPI cycle
      if (RDSR1_instruction_was_sent) begin
        next_state = RDSR1_get_byte;
        spi_master_multi_cycle_flag = 1'b0;

        // Blank SPI master transmission data
        spi_master_command = 8'h00;
      end

    end


    // Get byte
    RDSR1_get_byte: begin

      // Output data reflects command response
      output_data = RDSR1_response;

      // Move to next state when done
      if(flash_mem_interface_done) begin
        next_state = idle;
      end

      // Last byte received, disable SPI master, and start done pulse
      else if(RDSR1_response_was_received) begin
        flash_mem_interface_done = 1'b1;
        spi_master_enable = 1'b0;
      end

    end



    //##############################################
    // WREN
    //##############################################

    // Send Instruction
    WREN_send_instruction: begin

      // Flash interface is busy
      flash_mem_interface_done = 1'b0;

      // Set instruction and enable SPI master
      spi_master_command = WREN_command;
      spi_master_enable = 1'b1;

      // Move to next state when done, disable SPI master, and start done pulse
      if (WREN_instruction_was_sent) begin
        next_state = idle;
        flash_mem_interface_done = 1'b1;
        spi_master_enable = 1'b0;
        spi_master_command = 8'h00;
      end

    end



    //##############################################
    // READ3
    //##############################################

    // Send Instruction
    READ3_send_instruction: begin

      // Flash interface is busy
      flash_mem_interface_done = 1'b0;

      // Set instruction and enable SPI master for multi-cycle transfer
      spi_master_command = READ3_command;
      spi_master_enable = 1'b1;
      spi_master_multi_cycle_flag = 1'b1;

      // Move to next state when done
      if (READ3_instruction_was_sent) begin
        next_state = READ3_send_address;
      end

    end

    // Send address
    // READ3 command requires address of 3 bytes
    READ3_send_address: begin

      // Move to next state when done counting and reset counter
      if(READ3_send_address_counter == 3) begin
        next_state = READ3_get_bytes;
        reset_READ3_send_address_counter = 1'b1;
      end

      // Set address bytes
      else if(READ3_send_address_counter == 2) begin
        spi_master_command = address[7:0];
      end

      else if(READ3_send_address_counter == 1) begin
          spi_master_command = address[15:8];
      end

      else if(READ3_send_address_counter == 0) begin
          spi_master_command = address[23:16];
      end

    end

    // Get bytes
    READ3_get_bytes: begin

      // Deassert previous state's counter reset
      reset_READ3_send_address_counter = 1'b0;

      // Blank SPI master transmission data
      spi_master_command = 8'h00;

      // Output data reflects command response
      output_data = READ3_response;


      // Move to next state when done
      if(flash_mem_interface_done) begin
        next_state = idle;
      end

      // Last byte received, disable SPI master, reset counter, and start done pulse
      else if(READ3_get_bytes_counter == num_bytes) begin
        flash_mem_interface_done = 1'b1;
        reset_READ3_get_bytes_counter = 1;
        spi_master_enable = 1'b0;
      end

      // Second to last byte, inform SPI master of last SPI cycle
      else if(READ3_get_bytes_counter == num_bytes-1) begin
        spi_master_multi_cycle_flag = 1'b0;
      end

    end



    //##############################################
    // PP3
    //##############################################

    // Send Instruction
    PP3_send_instruction: begin

      // Flash interface is busy
      flash_mem_interface_done = 1'b0;

      // Set instruction and enable SPI master for multi-cycle transfer
      spi_master_command = PP3_command;
      spi_master_enable = 1'b1;
      spi_master_multi_cycle_flag = 1'b1;

      // Move to next state when done
      if (PP3_instruction_was_sent) begin
        next_state = PP3_send_address;
      end

    end

    // Send address
    // PP3 command requires address of 3 bytes
    PP3_send_address: begin

      // Move to next state when done counting and reset counter
      if(PP3_send_address_counter == 3) begin
        next_state = PP3_send_bytes;
        reset_PP3_send_address_counter = 1'b1;
      end

      // // Set address bytes
      // else if(PP3_send_address_counter == 2) begin
      //   spi_master_command = address[23:16];
      // end
      //
      // else if(PP3_send_address_counter == 1) begin
      //     spi_master_command = address[15:8];
      // end
      //
      // else if(PP3_send_address_counter == 0) begin
      //
      // end

      // Set address bytes
      else if(PP3_send_address_counter == 2) begin
        spi_master_command = address[7:0];
      end

      else if(PP3_send_address_counter == 1) begin
          spi_master_command = address[15:8];
      end

      else if(PP3_send_address_counter == 0) begin
          spi_master_command = address[23:16];
      end

    end

    // Send bytes
    PP3_send_bytes: begin

      // Deassert previous state's counter reset
      reset_PP3_send_address_counter = 1'b0;

      // // Blank SPI master transmission data
      // spi_master_command = 8'h00;

      // SPI transmission reflect input_data
      spi_master_command = input_data; // xfer the input data immediately every clk

      // Move to next state when done
      if(flash_mem_interface_done) begin
        next_state = idle;
      end

      // Last byte received, disable SPI master, reset counter, and start done pulse
      else if(PP3_send_bytes_counter == num_bytes[9:0]) begin
        flash_mem_interface_done = 1'b1;
        reset_PP3_send_bytes_counter = 1;
        spi_master_enable = 1'b0;
      end

      // Second to last byte, inform SPI master of last SPI cycle
      else if(PP3_send_bytes_counter == (num_bytes[9:0]-10'd1)) begin
        spi_master_multi_cycle_flag = 1'b0;
      end

    end



    //##############################################
    // PP4
    //##############################################

    // Send Instruction
    PP4_send_instruction: begin

      // Flash interface is busy
      flash_mem_interface_done = 1'b0;

      // Set instruction and enable SPI master for multi-cycle transfer
      spi_master_command = PP4_command;
      spi_master_enable = 1'b1;
      spi_master_multi_cycle_flag = 1'b1;

      // Move to next state when done
      if (PP4_instruction_was_sent) begin
        next_state = PP4_send_address;
      end

    end

    // Send address
    // PP4 command requires address of 4 bytes
    PP4_send_address: begin

      // Move to next state when done counting and reset counter
      if(PP4_send_address_counter == 4) begin
        next_state = PP4_send_bytes;
        reset_PP4_send_address_counter = 1'b1;
      end

      // Set address bytes
      else if(PP4_send_address_counter == 3) begin
        spi_master_command = address[31:24];
      end

      else if(PP4_send_address_counter == 2) begin
        spi_master_command = address[23:16];
      end

      else if(PP4_send_address_counter == 1) begin
          spi_master_command = address[15:8];
      end

      else if(PP4_send_address_counter == 0) begin
          spi_master_command = address[7:0];
      end

    end

    // Send bytes
    PP4_send_bytes: begin

      // Deassert previous state's counter reset
      reset_PP4_send_address_counter = 1'b0;

      // Blank SPI master transmission data
      spi_master_command = 8'h00;

      // SPI trnasmission reflect input_data
      spi_master_command = PP4_write_data;


      // Move to next state when done
      if(flash_mem_interface_done) begin
        next_state = idle;
      end

      // Last byte received, disable SPI master, reset counter, and start done pulse
      else if(PP4_send_bytes_counter == num_bytes) begin
        flash_mem_interface_done = 1'b1;
        reset_PP4_send_bytes_counter = 1;
        spi_master_enable = 1'b0;
      end

      // Second to last byte, inform SPI master of last SPI cycle
      else if(PP4_send_bytes_counter == num_bytes-1) begin
        spi_master_multi_cycle_flag = 1'b0;
      end

    end



    // Default state
    default: begin
        next_state=reset_state;
    end

  endcase


end

// State transition @ NEGEDGE clk
always@(negedge clk or negedge reset) begin
    if(!reset)
      current_state <= idle;
    else
      current_state <= next_state;
end





//##############################################
// SPI Master
//##############################################


spi_master MAS (
    .reset(spi_master_reset),
	  .clk(clk),
    .mlb(1'b1), // Always left shift, always MSB first
    .start_SPI_transfer(spi_master_enable),
    .multiple_byte_transfer(spi_master_multi_cycle_flag),
    .transmit_data(spi_master_command),
    .clk_to_sck_divisor(2'b00), // We want fastest SCK speed available
    .MISO(flash_SO),
    .SS(flash_CS1),
    .SCK(flash_SCK),
    .MOSI(flash_SI),
    .done(spi_master_done),
    .received_data(spi_master_response)
);

assign flash_mem_interface_spi_cycle_done = spi_master_done;







endmodule
