

module CRPII_FPGAFirmware(

  // System Inputs
  input clk,                                // (FPGA PIN E1)

  // Hercules SPI Master
  input wire hercules_SCK,                  // SPI clock            (FPGA Pin R1)
  input wire hercules_SS,                   // Chip select          (FPGA Pin P2)
  input wire hercules_MOSI,                 // Master Out Slave In  (FPGA Pin K2) *SBC Schematic is wrong
  output wire hercules_MISO,                // Master In Slave Out  (FPGA Pin L1) *SBC Schematic is wrong

  // Flash Memory (S70FL01GS) SPI Slave
  output wire flash_reset,                  // Flash HW reset       (FPGA Pin B1)
  output wire flash_SCK,                    // SPI clock            (FPGA Pin B4)
  output wire flash_CS1,                    // Chip select 1        (FPGA Pin C2)
  output wire flash_CS2,                    // Chip select 2        (FPGA Pin C3)
  output wire flash_SI,                     // Master Out Slave In  (FPGA Pin B3)
  input wire  flash_SO,                     // Master In Slave Out  (FPGA Pin D1)
  output wire flash_WP,                     // Write Protect        (FPGA Pin D3)
  output wire flash_HOLD,                   // hold_done            (FPGA Pin A2)

  // Camera 1 Interface
  output reg        camera_1_reset_bar,     // Camera reset     (FPGA Pin B10)
  output wire       camera_1_standby_bar,   // Standby          (FPGA Pin E11)
  output wire       camera_1_trigger,       // Trigger          (FPGA Pin A11)
  input wire        camera_1_pixel_clock,   // Pixel clock      (FPGA Pin D9)
  input wire        camera_1_FV,            // Frame valid flag (FPGA Pin C9)
  input wire        camera_1_LV,            // Line valid flag  (FPGA Pin A10)
  input wire [11:0] camera_1_pixel_in,      // Pixel data

  // LEDs
  output wire LED0,                          // (FPGA Pin L2)
  output reg LED1,                          // (FPGA Pin N1)
  output reg LED2,                          // (FPGA Pin K1)

  // Debug
  output camera_2_port_pin_2,               // (FPGA Pin M8)
  output camera_2_port_pin_4,               // (FPGA Pin T7)
  output camera_2_port_pin_8,               // (FPGA Pin N6)
  output camera_2_port_pin_10               // (FPGA Pin R6)
);


  // Debug
  assign camera_2_port_pin_2 = camera_1_LV;
  assign camera_2_port_pin_4 = flash_CS1;
  assign camera_2_port_pin_8 = flash_SI;
  assign camera_2_port_pin_10 = flash_SO;

  // assign camera_2_port_pin_8 = pll_is_locked;
  // assign camera_2_port_pin_10 = pll_clock;


  // Resets initialized to active reset condition
  reg flash_interface_reset = 1'b0;
  // reg camera_reset = 1'b0;


  wire flash_mem_interface_done;
  wire flash_mem_interface_spi_cycle_done;
  wire flash_mem_interface_output_valid;

  reg configure_flash = 1'b0;
  reg read_BAR = 1'b0;

  reg READ_ID = 1'b0;
  reg READ3 = 1'b0;
  reg WREN = 1'b0;
  reg RDSR1 = 1'b0;
  reg PP3 = 1'b0;

  reg write_flash = 1'b0;

  reg [31:0] address = 32'd0;
  reg [7:0] input_data;
  wire [7:0] output_data;
  reg [31:0] num_bytes = 32'd0;

  reg [7:0] latest_output_data;



  wire debug_out;

  S70FL01GS_Flash_Memory_Interface flash_mem(
    .reset(flash_interface_reset),
    .clk(pll_clock),
    .flash_reset(flash_reset),
    .flash_SCK(flash_SCK),
    .flash_CS1(flash_CS1),
    .flash_CS2(flash_CS2),
    .flash_SI(flash_SI),
    .flash_SO(flash_SO),
    .flash_WP(flash_WP),
    .flash_HOLD(flash_HOLD),
    .flash_mem_interface_done(flash_mem_interface_done),
    .flash_mem_interface_spi_cycle_done(flash_mem_interface_spi_cycle_done),
    .flash_mem_interface_output_valid(flash_mem_interface_output_valid),

    .READ_ID(READ_ID),
    .RDSR1(RDSR1),
    .WREN(WREN),
    .READ3(READ3),
    .PP3(PP3),

    .address(address),
    .num_bytes(num_bytes),

    .input_data(input_data),
    .output_data(output_data),

    .pll_is_locked(pll_is_locked),

    .debug_out(debug_out)
  );







  // FSM States

  reg [4:0] current_state, next_state;

  parameter reset_state               = 5'd0;
  parameter idle                      = 5'd1;

  parameter flash_READ_ID_wait        = 5'd2;
  parameter flash_READ_ID             = 5'd3;

  parameter flash_WREN_wait           = 5'd4;
  parameter flash_WREN                = 5'd5;

  parameter flash_RDSR1_wait          = 5'd6;
  parameter flash_RDSR1               = 5'd7;

  parameter flash_READ3_wait          = 5'd8;
  parameter flash_READ3               = 5'd9;

  parameter flash_PP3_wait            = 5'd10;
  parameter flash_PP3                 = 5'd11;
  parameter flash_PP3_RDSR1_wait      = 5'd12;
  parameter flash_PP3_RDSR1           = 5'd13;
  parameter flash_PP3_RDSR1_check     = 5'd14;





  reg PLL_reset = 1'b1; // Initialized to active reset
  wire pll_is_locked;

  wire pll_clock;

  assign LED0 = ~pll_is_locked;

  PLL PLL(
    .areset(PLL_reset),
    .inclk0(clk),
    .c0(pll_clock),
    .locked(pll_is_locked)
  );


  reg [23:0] powerup_reset_counter = 24'd0;

  reg [31:0] wait_counter = 32'd0;

  reg flash_READ_ID_done = 1'b0;
  reg reset_flash_READ_ID_done = 1'b0;

  reg flash_WREN_done = 1'b0;
  reg reset_flash_WREN_done = 1'b0;

  reg flash_RDSR1_done = 1'b0;
  reg reset_flash_RDSR1_done = 1'b0;

  reg flash_READ3_done = 1'b0;
  reg reset_flash_READ3_done = 1'b0;

  reg flash_PP3_done = 1'b0;
  reg reset_flash_PP3_done = 1'b0;

  reg flash_PP3_RDSR1_done = 1'b0;
  reg reset_flash_PP3_RDSR1_done = 1'b0;
  reg [7:0] flash_PP3_RDSR1_response = 8'd0;


  // State Machine
  always @(posedge pll_clock) begin

    if(pll_is_locked) begin

    next_state=current_state;

    // State transition definitions
    case(current_state)

      // RESET
      reset_state: begin

        // Move to idle when done
        if (powerup_reset_counter == 24'hFFFFFF) begin

          if(pll_is_locked)
            next_state = idle;

          // Take flash interface in reset
          flash_interface_reset = 1'b1;

          // Take camera out of reset
          camera_1_reset_bar = 1'b1;

          // // Take PLL out of reset
          // PLL_reset = 1'b0;

        end

        // Reset
        else begin
          // Increment counter
          powerup_reset_counter = powerup_reset_counter + 1;

          // Hold flash interface in reset
          flash_interface_reset = 1'b0;

          // Hold camera in reset
          camera_1_reset_bar = 1'b0;

          // // Hold PLL in reset
          // PLL_reset = 1'b1;
        end

      end


      // IDLE
      idle: begin

        if (wait_counter == 32'h00100000) begin
          next_state = flash_READ_ID_wait;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end

      end



      flash_READ_ID_wait: begin

        // Deassert previuos state's done reset flag
        // reset_flash_WREN_done = 1'b0;

        // if (wait_counter > 32'b01011001011010000010111100000000) begin
        if (wait_counter == 32'h00000800) begin
          next_state = flash_READ_ID;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end
      end


      // Trigger read of flash ID
      flash_READ_ID: begin

        // LED1 = 1'b0; // On
        // LED2 = 1'b1; // Off

        if(flash_READ_ID_done) begin
          reset_flash_READ_ID_done = 1'b1;
          READ_ID = 1'b0;
          next_state = flash_WREN_wait;
        end
        else begin
          READ_ID = 1'b1;
          // next_state = flash_READ_ID;
        end
      end



      flash_READ3_wait: begin

        // Deassert previuos state's done reset flag
        reset_flash_READ_ID_done = 1'b0;

        if (wait_counter == 32'h00000100) begin
          next_state = flash_READ3;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end
      end


      // Trigger read of status register 1
      flash_READ3: begin

        // LED1 = 1'b1; // Off
        // LED2 = 1'b0; // On

        if(flash_READ3_done) begin
          reset_flash_READ3_done = 1'b1;
          READ3 = 1'b0;
          address = 32'h00000000;
          num_bytes = 32'h00000000;

          next_state = flash_WREN_wait;
        end
        else begin
          READ3 = 1'b1;
          address = 32'h04030201;
          num_bytes = 32'd512;
        end
      end


      flash_WREN_wait: begin

        // Deassert previuos state's done reset flag
        // reset_flash_READ3_done = 1'b0;
        reset_flash_READ_ID_done = 1'b0;


        if (wait_counter == 32'h00000100) begin
          next_state = flash_WREN;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end
      end

      // Trigger read of status register 1
      flash_WREN: begin

        // LED1 = 1'b1; // Off
        // LED2 = 1'b0; // On

        if(flash_WREN_done) begin
          reset_flash_WREN_done = 1'b1;
          WREN = 1'b0;
          next_state = flash_RDSR1_wait;
        end
        else begin
          WREN = 1'b1;
        end
      end


      flash_RDSR1_wait: begin

        // Deassert previuos state's done reset flag
        reset_flash_WREN_done = 1'b0;

        if (wait_counter == 32'h00000100) begin
          next_state = flash_RDSR1;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end
      end

      // Trigger reaad of status register 1
      flash_RDSR1: begin

        // LED1 = 1'b1; // Off
        // LED2 = 1'b0; // On

        if(flash_RDSR1_done) begin
          reset_flash_RDSR1_done = 1'b1;
          RDSR1 = 1'b0;
          next_state = flash_PP3_wait;
        end
        else begin
          RDSR1 = 1'b1;
          // next_state = flash_READ_ID;
        end
      end


      flash_PP3_wait: begin

        // Deassert previuos state's done reset flag
        reset_flash_RDSR1_done = 1'b0;

        if (wait_counter == 32'h00000300) begin
          next_state = flash_PP3;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end
      end

      // Trigger page program
      flash_PP3: begin

        LED1 = 1'b1; // Off
        LED2 = 1'b0; // On

        if(flash_PP3_done) begin
          reset_flash_PP3_done = 1'b1;
          PP3 = 1'b0;
          address = 32'h00000000;
          num_bytes = 32'd0;

          input_data = 8'h00;

          next_state = flash_PP3_RDSR1_wait;
        end
        else begin
          PP3 = 1'b1;
          address = 32'h04030201;
          num_bytes = 32'd512;

          input_data = input_data + 1;
        end
      end


      flash_PP3_RDSR1_wait: begin

        // Deassert previuos state's done reset flag
        reset_flash_PP3_done = 1'b0;

        if (wait_counter == 32'h00000010) begin
          next_state = flash_PP3_RDSR1;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end
      end

      // Trigger read of status register 1
      flash_PP3_RDSR1: begin


        if(flash_PP3_RDSR1_done) begin
          flash_PP3_RDSR1_response = output_data;

          reset_flash_PP3_RDSR1_done = 1'b1;
          RDSR1 = 1'b0;
          next_state = flash_PP3_RDSR1_check;
        end
        else begin
          RDSR1 = 1'b1;
        end
      end

      // RDSR1 until SR1[0] (Write in progress bit) is equal to 0
      flash_PP3_RDSR1_check: begin

        // Deassert previuos state's done reset flag
        reset_flash_PP3_RDSR1_done = 1'b0;

        if(flash_PP3_RDSR1_response[0] == 1'b0)
          next_state = flash_READ_ID_wait;
        else
          next_state = flash_PP3_RDSR1_wait;

      end



      default: begin

        next_state = reset_state;

      end

    endcase

    end
  end

  // State transition @ NEGEDGE clk
  always@(negedge pll_clock) begin
        if(pll_is_locked) begin
      current_state <= next_state;
  end



  // READ_ID Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_READ_ID_done) begin
    if(reset_flash_READ_ID_done) begin
      flash_READ_ID_done = 1'b0;
    end
    else begin
      if(current_state == flash_READ_ID)
        flash_READ_ID_done = 1'b1;
    end
  end


  // WREN Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_WREN_done) begin
    if(reset_flash_WREN_done) begin
      flash_WREN_done = 1'b0;
    end
    else begin
      if(current_state == flash_WREN)
        flash_WREN_done = 1'b1;
    end
  end


  // RDSR1 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_RDSR1_done) begin
    if(reset_flash_RDSR1_done) begin
      flash_RDSR1_done = 1'b0;
    end
    else begin
      if(current_state == flash_RDSR1)
        flash_RDSR1_done = 1'b1;
    end
  end


  // READ3 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_READ3_done) begin
    if(reset_flash_READ3_done) begin
      flash_READ3_done = 1'b0;
    end
    else begin
      if(current_state == flash_READ3)
        flash_READ3_done = 1'b1;
    end
  end


  // PP3 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_PP3_done) begin
    if(reset_flash_PP3_done) begin
      flash_PP3_done = 1'b0;
    end
    else begin
      if(current_state == flash_PP3)
        flash_PP3_done = 1'b1;
    end
  end

  // PP3 RDSR1 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_PP3_RDSR1_done) begin
    if(reset_flash_PP3_RDSR1_done) begin
      flash_PP3_RDSR1_done = 1'b0;
    end
    else begin
      if(current_state == flash_PP3_RDSR1)
        flash_PP3_RDSR1_done = 1'b1;
        // flash_PP3_RDSR1_response = output_data;
    end
  end




  assign camera_1_standby_bar = 1'b1;




  reg PLL_in_powerup_reset = 1'b1; // Start in reset
  reg [23:0] PLL_powerup_reset_counter = 24'd0;

  always@(posedge clk) begin
    if (PLL_in_powerup_reset) begin

      // Reset active
      PLL_reset = 1'b1;
      PLL_powerup_reset_counter = PLL_powerup_reset_counter + 1;

      if (PLL_powerup_reset_counter == 24'hFFFFFF) begin
        PLL_in_powerup_reset = 1'b0;
      end
    end

    else begin
      // Deactivate reset
      PLL_reset = 1'b0;
    end
  end








endmodule
