

module CRPII_FPGAFirmware_v2(

  // System Inputs
  input clk,                                // (FPGA PIN E1)

  // Hercules SPI Master
  input wire hercules_SCK,                  // SPI clock            (FPGA Pin R1)
  input wire hercules_SS,                   // Chip select          (FPGA Pin P2)
  input wire hercules_MOSI,                 // Master Out Slave In  (FPGA Pin K2) *SBC Schematic is wrong
  output wire hercules_MISO,                // Master In Slave Out  (FPGA Pin L1) *SBC Schematic is wrong

  // To Hercules
  output flash_SPI_controller_indicator,    // Indicates FPGA is controlling flash memory (FPGA Pin P1)

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
  output reg  camera_1_reset_bar,           // Camera reset     (FPGA Pin B10)
  output wire camera_1_standby_bar,         // Standby          (FPGA Pin E11)
  output wire camera_1_trigger,             // Trigger          (FPGA Pin A11)
  input wire camera_1_pixel_clock,          // Pixel clock      (FPGA Pin D9)
  input wire camera_1_FV,                   // Frame valid flag (FPGA Pin C9)
  input wire camera_1_LV,                   // Line valid flag  (FPGA Pin A10)
  input wire [11:0] camera_1_pixel_in,      // Pixel data

  // LEDs
  output wire LED0,                         // Red LED    (FPGA Pin L2)
  output reg LED1,                         // Green LED  (FPGA Pin N1)
  output reg LED2,                         // Blue LED   (FPGA Pin K1)

  // Debug
  output wire camera_2_port_pin_2,           // (FPGA Pin M8)
  output wire camera_2_port_pin_4,           // (FPGA Pin T7)
  output wire camera_2_port_pin_8,           // (FPGA Pin N6)
  output wire camera_2_port_pin_10           // (FPGA Pin R6)
);



  wire FIFO_is_empty;
  wire FIFO_is_full;
  reg FIFO_read_req = 1'b0;
  reg FIFO_write_enable = 1'b0;

  reg capturing_camera_frames = 1'b0;

  reg waiting_for_fresh_frame = 1'b0;
  reg done_waiting_for_fresh_frame = 1'b0;



  // Debug
  assign camera_2_port_pin_2 = flash_SCK;
  assign camera_2_port_pin_4 = flash_CS1;
  assign camera_2_port_pin_8 = flash_SI;
  assign camera_2_port_pin_10 = flash_SO;

  // assign camera_2_port_pin_2 = camera_1_FV;
  // assign camera_2_port_pin_4 = flash_CS1;
  // assign camera_2_port_pin_8 = camera_1_LV;
  // assign camera_2_port_pin_10 = new_camera_FV;


  // assign camera_2_port_pin_8 = pll_is_locked;
  // assign camera_2_port_pin_10 = pll_clock;

  // Ensure camera is not in standby
  assign camera_1_standby_bar = 1'b1;



  // assign LED1 = 1'b1;
  // assign LED2 = 1'b1;



  wire FPGA_driven_flash_SCK;
  wire FPGA_driven_flash_CS1;
  wire FPGA_driven_flash_MOSI;
  wire FPGA_MISO;


  // SPI to flash has two possible sources for an SPI master interface
  //  FPGA driven signals use the S70FL01GS_Flash_Memory_Interface to control SPI interface
  //  Hercules driven signals are controlled off-chip on Hercules, FPGA provides bridge
  reg flash_SPI_controller = 1'b0;

  parameter hercules_controls_flash   = 1'b0; // LED0 On
  parameter FPGA_controls_flash       = 1'b1; // LED0 Off

  assign LED0 = flash_SPI_controller;

  // Output to Hercules indicating the current flash SPI controller
  assign flash_SPI_controller_indicator = flash_SPI_controller;

  // Assign inputs to flash SPI interface
  assign flash_SCK      = flash_SPI_controller ? FPGA_driven_flash_SCK  : hercules_SCK;
  assign flash_CS1      = flash_SPI_controller ? FPGA_driven_flash_CS1  : hercules_SS;
  assign flash_SI       = flash_SPI_controller ? FPGA_driven_flash_MOSI : hercules_MOSI;

  // Assign outputs from flash SPI interface
  assign hercules_MISO  = flash_SPI_controller ? 1'b0     : flash_SO;
  assign FPGA_MISO      = flash_SPI_controller ? flash_SO : 1'b0;




  // Hercules SPI Slave Listener Interface
  //  - Attaches to Hercules SPI interface lines and listens for camera triggers only

  reg hercules_SPI_listener_reset = 1'b0; // Initialized to active reset
  wire hercules_SPI_listener_done;
  wire [7:0] hercules_SPI_listener_rx_data;

  reg [7:0] latest_hercules_SPI_listener_rx_data = 8'h00;

  // Arbitrary value that is NOT a listed command for flash memory interface
  reg [7:0] hercules_take_picture_command = 8'hFE;

  spi_slave hercules_SPI_listener (
    .reset(hercules_SPI_listener_reset),
    .ten(1'b0), // Never transmit
    .tdata(8'h00),
    .mlb(1'b1),
    .SS(hercules_SS),
    .SCK(hercules_SCK),
    .MOSI(hercules_MOSI),
    .MISO(),
    .done(hercules_SPI_listener_done),
    .rdata(hercules_SPI_listener_rx_data)
  );

  // always@(posedge hercules_SPI_listener_done) begin
  //
  //
  //
  // end





  // PLL - Doubles system frequency
  //   For a little bit of a clock boost
  reg PLL_reset = 1'b1; // Initialized to active reset
  wire pll_is_locked;
  wire pll_clock;

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

  // assign LED0 = ~pll_is_locked;

  PLL PLL(
    .areset(PLL_reset),
    .inclk0(clk),
    .c0(pll_clock),
    .locked(pll_is_locked)
  );



  // Resets initialized to active reset condition
  reg flash_interface_reset = 1'b0;

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
  reg PP4 = 1'b0;

  reg write_flash = 1'b0;

  reg [31:0] address = 32'd0;
  reg [7:0] input_data;
  wire [7:0] output_data;
  reg [31:0] num_bytes = 32'd0;

  reg [7:0] latest_output_data;

  wire debug_out;

  S70FL01GS_Flash_Memory_Interface flash_mem(
    .reset(flash_interface_reset),
    .clk(clk),
    .flash_reset(flash_reset),
    .flash_SCK(FPGA_driven_flash_SCK),
    .flash_CS1(FPGA_driven_flash_CS1),
    .flash_CS2(flash_CS2),
    .flash_SI(FPGA_driven_flash_MOSI),
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
    .PP4(PP4),

    .address(address),
    .num_bytes(num_bytes),

    .input_data(input_data),
    .output_data(output_data),

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
  parameter flash_WREN_RDSR1_wait     = 5'd6;
  parameter flash_WREN_RDSR1          = 5'd7;
  parameter flash_WREN_RDSR1_check    = 5'd8;

  parameter flash_RDSR1_wait          = 5'd9;
  parameter flash_RDSR1               = 5'd10;

  parameter flash_READ3_wait          = 5'd11;
  parameter flash_READ3               = 5'd12;

  parameter flash_PP3_wait            = 5'd13;
  parameter flash_PP3                 = 5'd14;
  parameter flash_PP3_RDSR1_wait      = 5'd15;
  parameter flash_PP3_RDSR1           = 5'd16;
  parameter flash_PP3_RDSR1_check     = 5'd17;

  parameter wait_for_new_camera_FV    = 5'd18;
  parameter wait_for_new_camera_LV    = 5'd19;

  parameter wait_to_switchover_flash_control    = 5'd20;

  parameter flash_PP3_check_FIFO      = 5'd21;

  parameter flash_RDSR1_check_WIP     = 5'd22;

  parameter flash_PP3_repeat_or_exit     = 5'd23;


  parameter flash_PP4_wait            = 5'd24;
  parameter flash_PP4                 = 5'd25;
  parameter flash_PP4_RDSR1_wait      = 5'd26;
  parameter flash_PP4_RDSR1           = 5'd27;
  parameter flash_PP4_RDSR1_check     = 5'd28;
  parameter flash_PP4_repeat_or_exit  = 5'd29;


  reg [23:0] powerup_reset_counter = 24'd0;

  reg [31:0] wait_counter = 32'd0;

  reg flash_READ_ID_done = 1'b0;
  reg reset_flash_READ_ID_done = 1'b0;

  reg flash_WREN_done = 1'b0;
  reg reset_flash_WREN_done = 1'b0;

  reg flash_WREN_RDSR1_done = 1'b0;
  reg reset_flash_WREN_RDSR1_done = 1'b0;
  reg [7:0] flash_WREN_RDSR1_response = 8'd0;

  reg flash_RDSR1_done = 1'b0;
  reg reset_flash_RDSR1_done = 1'b0;
  reg [7:0] flash_RDSR1_response = 8'd0;


  reg flash_READ3_done = 1'b0;
  reg reset_flash_READ3_done = 1'b0;

  reg flash_PP3_done = 1'b0;
  reg reset_flash_PP3_done = 1'b0;

  reg reset_PP3_FIFO_read_req = 1'b0;

  reg flash_PP3_RDSR1_done = 1'b0;
  reg reset_flash_PP3_RDSR1_done = 1'b0;
  reg [7:0] flash_PP3_RDSR1_response = 8'd0;

  reg flash_PP4_done = 1'b0;
  reg reset_flash_PP4_done = 1'b0;

  reg flash_PP4_RDSR1_done = 1'b0;
  reg reset_flash_PP4_RDSR1_done = 1'b0;
  reg [7:0] flash_PP4_RDSR1_response = 8'd0;

  reg new_camera_FV = 1'b0;
  reg reset_new_camera_FV = 1'b0;

  reg new_camera_LV = 1'b0;
  reg reset_new_camera_LV = 1'b0;

  // State Machine
  always @(posedge clk) begin


    next_state=current_state;

    // State transition definitions
    case(current_state)

      // RESET
      reset_state: begin

        // Move to idle when done
        if (powerup_reset_counter == 24'hFFFFFF) begin

          next_state = idle;

          // Take Hercules SPI listener interface out of reset
          hercules_SPI_listener_reset = 1'b1;

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

          // Hold Hercules SPI listener interface in reset
          hercules_SPI_listener_reset = 1'b0;

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

        // // Wait time
        // if (wait_counter == 32'h00100000) begin
        //   next_state = flash_READ_ID_wait;
        //   wait_counter = 0;
        // end
        // else begin
        //   wait_counter = wait_counter + 1;
        // end

        // reset_flash_WREN_done = 1'b0;

        // Previous state reset of counter reset
        reset_PP3_command_counter = 1'b0;

        // Previous state reset of counter reset
        reset_PP4_command_counter = 1'b0;

        reset_FIFO_bytes_written_counter = 1'b0;

        // Wait for take picture trigger
        if(hercules_SPI_listener_rx_data == hercules_take_picture_command) begin
          next_state = flash_RDSR1_wait;
          // next_state = wait_for_new_camera_FV;


          // FPGA takes over flash memory control
          flash_SPI_controller = FPGA_controls_flash;

          // Hold Hercules SPI listener interface in reset
          hercules_SPI_listener_reset = 1'b0;

          capturing_camera_frames = 1'b1;

        end

        // Otherwise, hercules controls flash memory
        else begin

          // Hercules holds flash memory control
          flash_SPI_controller = hercules_controls_flash;

          // Take Hercules SPI listener interface out of reset
          hercules_SPI_listener_reset = 1'b1;


          capturing_camera_frames = 1'b0;
          done_waiting_for_fresh_frame = 1'b0;
        end


      end




      // Make sure any previous commands are finished
      flash_RDSR1_wait: begin

        if (wait_counter == 32'h00000100) begin
          next_state = flash_RDSR1;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end
      end

      // Trigger read of status register 1
      flash_RDSR1: begin

        if(flash_RDSR1_done) begin
          flash_RDSR1_response = output_data;

          reset_flash_RDSR1_done = 1'b1;
          RDSR1 = 1'b0;
          next_state = flash_RDSR1_check_WIP;
        end
        else begin
          RDSR1 = 1'b1;
        end
      end

      // RDSR1 until SR1[0] (Write in progress bit) is equal to 0
      flash_RDSR1_check_WIP: begin

        // Deassert previous state's done reset flag
        reset_flash_RDSR1_done = 1'b0;

        if(flash_RDSR1_response[0] == 1'b0)
          next_state = wait_for_new_camera_FV;
        else
          next_state = flash_RDSR1_wait;

      end


      // We need to wait for a FV assertion to get the start of an image
      wait_for_new_camera_FV: begin

        LED1 = 1'b0; // On

        if (new_camera_FV) begin
          reset_new_camera_FV = 1'b1;
          next_state = flash_WREN_wait;

          done_waiting_for_fresh_frame = 1'b1;
        end

      end


      // wait_for_new_camera_LV: begin
      //
      //   LED1 = 1'b0; // On
      //
      //
      //   // Deassert previous state's done reset flag
      //   reset_new_camera_FV = 1'b0;
      //
      //   if (camera_1_LV) begin
      //     reset_new_camera_LV = 1'b1;
      //     next_state = flash_WREN_wait;
      //   end
      //
      // end


      // wait_for_FIFO: begin
      //
      //   LED1 = 1'b0; // On
      //
      //   // Deassert previous state's done reset flag
      //   reset_new_camera_FV = 1'b0;
      //
      //   if (camera_1_LV) begin
      //     reset_new_camera_LV = 1'b1;
      //     next_state = flash_WREN_wait;
      //   end
      //
      // end


      flash_WREN_wait: begin

        LED1 = 1'b1; // Off


        // Deassert previous state's done reset flag
        // reset_flash_READ3_done = 1'b0;
        // reset_flash_READ_ID_done = 1'b0;

        reset_new_camera_FV = 1'b0;
        // reset_new_camera_LV = 1'b0;

        if (wait_counter == 32'h00000000) begin
          next_state = flash_WREN;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end
      end

      // Trigger read of status register 1
      flash_WREN: begin

        LED2 = 1'b0; // On

        // LED1 = 1'b1; // Off
        // LED2 = 1'b0; // On

        if(flash_WREN_done) begin
          reset_flash_WREN_done = 1'b1;
          WREN = 1'b0;
          next_state = flash_WREN_RDSR1_wait;
        end
        else begin
          WREN = 1'b1;
        end
      end


      // Verify WREN
      flash_WREN_RDSR1_wait: begin

        LED2 = 1'b1; // Off


        // Deassert previous state's done reset flag
        reset_flash_WREN_done = 1'b0;

        if (wait_counter == 32'h00000000) begin
          next_state = flash_WREN_RDSR1;
          wait_counter = 0;
        end
        else begin
          wait_counter = wait_counter + 1;
        end
      end

      // Trigger read of status register 1
      flash_WREN_RDSR1: begin


        if(flash_WREN_RDSR1_done) begin
          flash_WREN_RDSR1_response = output_data;

          reset_flash_WREN_RDSR1_done = 1'b1;
          RDSR1 = 1'b0;
          next_state = flash_WREN_RDSR1_check;
        end
        else begin
          RDSR1 = 1'b1;
        end
      end


      // RDSR1 until SR1[1] (Write enable latch bit) is equal to 1
      flash_WREN_RDSR1_check: begin

        // Deassert previous state's done reset flag
        reset_flash_WREN_RDSR1_done = 1'b0;

        if(flash_WREN_RDSR1_response[1] == 1'b1)
          next_state = flash_PP3_wait;
        else
          next_state = flash_WREN_RDSR1_wait;

      end




      flash_PP3_wait: begin

        // LED2 = 1'b0; // On

        // if(PP3_command_counter < 6) begin
        //   if (FIFO_bytes_written_counter > 10'd512) begin
        //     next_state = flash_PP3;
        //     // reset_FIFO_bytes_written_counter = 1;
        //
        //     num_bytes = 32'd512;
        //   end
        // end

        if(FIFO_bytes_written_counter > 12'd0)
          LED2 = 1'b0; // On
        else
          LED2 = 1'b1; // Off

        // Wait for full FIFO
        if(PP3_command_counter == 0) begin
          if (FIFO_bytes_written_counter > 10'd512) begin
          // if (!FIFO_is_empty) begin

            next_state = flash_PP3;
            num_bytes = 32'd512;

            // Image start address
            address = 32'h00000000;

          end
        end

        else begin
            next_state = flash_PP3;

            if(PP3_command_counter == 5)
                num_bytes = 32'd32;
            else
                num_bytes = 32'd512;

        end
        // // Otherwise just ensure FIFO is not empty
        // else if(!FIFO_is_empty) begin

      // // Last cycle will only need 32 bytes
      // else if(PP3_command_counter == 5 && (FIFO_bytes_written_counter > 10'd32)) begin
      //   next_state = flash_PP3;
      //   num_bytes = 32'd32;
      // end
      //
      // else if(FIFO_bytes_written_counter > 10'd512) begin
      //   next_state = flash_PP3;
      //   num_bytes = 32'd512;
      // end

        // end
      end


      // Trigger page program
      flash_PP3: begin

        // Deassert previous state's counter reset flag
        // reset_FIFO_bytes_written_counter = 1'b0;

        // LED2 = 1'b1; // Off
        // LED2 = 1'b0; // On

        if(flash_PP3_done) begin
          reset_flash_PP3_done = 1'b1;
          reset_PP3_command_SPI_cycle_counter = 1'b1;
          reset_PP3_FIFO_read_req = 1'b1;

          PP3 = 1'b0;
          address = address + 32'h00000200;
          // num_bytes = 32'd0;

          // FIFO_read_req = 1'b0;
          // input_data = 8'h11;

          next_state = flash_PP3_RDSR1_wait;
        end
        else begin
          PP3 = 1'b1;
          // address = 32'h00040000;
          // num_bytes = 32'd512;


          // // TODO
          // // Write data not valid until PP3 instruction and 3 byte address have been sent
          // //    Input data should be ready on the 4th clock
          // if(PP3_command_SPI_cycle_counter >= 4) begin
          //   FIFO_read_req = 1'b1;
          //   // input_data = byte_to_flash; //FIFO_is_empty ? 8'h11 : byte_to_flash;
          //   // input_data = input_data + 1;
          //
          // end
          // else
          //   FIFO_read_req = 1'b0;

          // input_data = input_data + 1;

        end
      end


      flash_PP3_RDSR1_wait: begin

        // Deassert previous state's done reset flag
        reset_flash_PP3_done = 1'b0;

        // Deassert previous state's SPI cyc;e counter reset flag
        reset_PP3_command_SPI_cycle_counter = 1'b0;

        reset_PP3_FIFO_read_req = 1'b0;

        if (wait_counter == 32'h00000000) begin
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

        // Deassert previous state's done reset flag
        reset_flash_PP3_RDSR1_done = 1'b0;

        if(flash_PP3_RDSR1_response[0] == 1'b0)
          next_state = flash_PP3_repeat_or_exit;
        else
          next_state = flash_PP3_RDSR1_wait;

      end


      // Write data to flash from FIFO as long as its not empty
      flash_PP3_check_FIFO: begin

        if(FIFO_is_empty) begin
          next_state = idle;

        end
        else
          next_state = flash_WREN_wait;

      end

      // Write data to flash from FIFO as long as its not empty
      flash_PP3_repeat_or_exit: begin

        if(PP3_command_counter == 6) begin
        // if(PP3_command_counter == 2) begin
          next_state = idle;

          reset_PP3_command_counter = 1'b1;
          reset_FIFO_bytes_written_counter = 1'b1;

        end

        // Otherwise we need another write to flash
        else
          next_state = flash_WREN_wait;

      end





      //
      // flash_PP4_wait: begin
      //
      //   // Wait for full FIFO
      //   if(PP4_command_counter == 0) begin
      //     if (FIFO_bytes_written_counter == 12'd2592) begin
      //       next_state = flash_PP4;
      //       num_bytes = 32'd512;
      //
      //       // Image start address
      //       address = 32'h01000000;
      //
      //     end
      //   end
      //
      //   else begin
      //       next_state = flash_PP4;
      //
      //       if(PP4_command_counter == 5)
      //           num_bytes = 32'd32;
      //       else
      //           num_bytes = 32'd512;
      //
      //   end
      // end
      //
      //
      // // Trigger page program
      // flash_PP4: begin
      //
      //   // Deassert previous state's counter reset flag
      //   // reset_FIFO_bytes_written_counter = 1'b0;
      //
      //   // LED1 = 1'b1; // Off
      //   // LED2 = 1'b0; // On
      //
      //   if(flash_PP4_done) begin
      //     reset_flash_PP4_done = 1'b1;
      //     reset_PP4_command_SPI_cycle_counter = 1'b1;
      //
      //     PP4 = 1'b0;
      //     address = address + 32'h00000200;
      //     // num_bytes = 32'd0;
      //
      //     FIFO_read_req = 1'b0;
      //     input_data = 8'h11;
      //
      //     next_state = flash_PP4_RDSR1_wait;
      //   end
      //   else begin
      //     PP4 = 1'b1;
      //     // address = 32'h00040000;
      //     // num_bytes = 32'd512;
      //
      //
      //     // TODO
      //     // Write data not valid until PP4 instruction and 3 byte address have been sent
      //     //    Input data should be ready on the 4th clock
      //     // if(PP4_command_SPI_cycle_counter >= 4) begin
      //     //   FIFO_read_req = 1'b1;
      //     //   input_data = byte_to_flash; //FIFO_is_empty ? 8'h11 : byte_to_flash;
      //     // end
      //     // else
      //     //   FIFO_read_req = 1'b0;
      //
      //     // input_data = input_data + 1;
      //
      //   end
      // end
      //
      //
      // flash_PP4_RDSR1_wait: begin
      //
      //   // Deassert previous state's done reset flag
      //   reset_flash_PP4_done = 1'b0;
      //
      //   // Deassert previous state's SPI cyc;e counter reset flag
      //   reset_PP4_command_SPI_cycle_counter = 1'b0;
      //
      //   if (wait_counter == 32'h00000000) begin
      //     next_state = flash_PP4_RDSR1;
      //     wait_counter = 0;
      //   end
      //   else begin
      //     wait_counter = wait_counter + 1;
      //   end
      // end
      //
      // // Trigger read of status register 1
      // flash_PP4_RDSR1: begin
      //
      //   if(flash_PP4_RDSR1_done) begin
      //     flash_PP4_RDSR1_response = output_data;
      //
      //     reset_flash_PP4_RDSR1_done = 1'b1;
      //     RDSR1 = 1'b0;
      //     next_state = flash_PP4_RDSR1_check;
      //   end
      //   else begin
      //     RDSR1 = 1'b1;
      //   end
      // end
      //
      // // RDSR1 until SR1[0] (Write in progress bit) is equal to 0
      // flash_PP4_RDSR1_check: begin
      //
      //   // Deassert previous state's done reset flag
      //   reset_flash_PP4_RDSR1_done = 1'b0;
      //
      //   if(flash_PP4_RDSR1_response[0] == 1'b0)
      //     next_state = flash_PP4_repeat_or_exit;
      //   else
      //     next_state = flash_PP4_RDSR1_wait;
      //
      // end
      //
      //
      // // // Write data to flash from FIFO as long as its not empty
      // // flash_PP4_check_FIFO: begin
      // //
      // //   if(FIFO_is_empty) begin
      // //     next_state = idle;
      // //
      // //     // address = 32'h00000000;
      // //
      // //   end
      // //   else
      // //     next_state = flash_WREN_wait;
      // //
      // // end
      //
      // // Write data to flash from FIFO as long as its not empty
      // flash_PP4_repeat_or_exit: begin
      //
      //   // if(PP4_command_counter == 6) begin
      //   if(PP4_command_counter == 2) begin
      //     next_state = idle;
      //     // address = 32'h00000000;
      //
      //     reset_PP4_command_counter = 1'b1;
      //
      //     reset_FIFO_bytes_written_counter = 1'b1;
      //
      //
      //   end
      //
      //   // Otherwise we need another write to flash
      //   else
      //     next_state = flash_WREN_wait;
      //
      // end




      //################
      // UNUSED STATES


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

          next_state = idle;
        end
        else begin
          READ3 = 1'b1;
          address = 32'h04030201;
          num_bytes = 32'd512;
        end
      end





      default: begin

        next_state = reset_state;

      end

    endcase

  end

  // State transition @ NEGEDGE clk
  always@(negedge clk) begin
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

  // WREN RDSR1 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_WREN_RDSR1_done) begin
    if(reset_flash_WREN_RDSR1_done) begin
      flash_WREN_RDSR1_done = 1'b0;
    end
    else begin
      if(current_state == flash_WREN_RDSR1)
        flash_WREN_RDSR1_done = 1'b1;
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


  // FIFO control and PP3 input data
  always@(posedge flash_mem_interface_spi_cycle_done or posedge reset_PP3_FIFO_read_req) begin

    if(reset_PP3_FIFO_read_req)
      FIFO_read_req = 1'b0;

    else begin
      if(current_state == flash_PP3 && PP3_command_SPI_cycle_counter >= 4) begin
        FIFO_read_req = 1'b1;
        input_data = byte_to_flash;
      end
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
    end
  end



  // PP4 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_PP4_done) begin
    if(reset_flash_PP4_done) begin
      flash_PP4_done = 1'b0;
    end
    else begin
      if(current_state == flash_PP4)
        flash_PP4_done = 1'b1;
    end
  end

  // PP4 RDSR1 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_PP4_RDSR1_done) begin
    if(reset_flash_PP4_RDSR1_done) begin
      flash_PP4_RDSR1_done = 1'b0;
    end
    else begin
      if(current_state == flash_PP4_RDSR1)
        flash_PP4_RDSR1_done = 1'b1;
    end
  end









  parameter phase_0 = 4'd0;
  parameter phase_1 = 4'd1;
  parameter phase_2 = 4'd2;
  parameter phase_3 = 4'd3;
  parameter phase_4 = 4'd4;
  parameter phase_5 = 4'd5;
  parameter phase_6 = 4'd6;
  parameter phase_7 = 4'd7;


  reg [3:0] interleave_phase = phase_0;

  reg [3:0] line_counter = 4'd7;


  wire [7:0] byte_to_flash;

  // Camera pixel FIFO


  // assign pixel_is_valid = ! FIFO_is_empty && rx_Valid_out;
  // assign FIFO_read_req = ! FIFO_is_empty && rx_Valid_out ;

  FIFO CAM_FIFO
  (
    	.data( camera_1_pixel_in[11:4] ),
      .wrclk( !camera_1_pixel_clock ),
    	.wrreq( camera_1_LV && capturing_camera_frames && done_waiting_for_fresh_frame && (line_counter == interleave_phase)), //FIFO_write_enable ), // TODO

      .rdclk( flash_mem_interface_spi_cycle_done ),
      .rdreq( FIFO_read_req ), // TODO
    	.q( byte_to_flash ),

    	.rdempty( FIFO_is_empty ),
    	.wrfull( FIFO_is_full )
  );


  // always@(posedge camera_0_LV) begin
  //
  //   if(capturing_camera_frames) begin
  //
  //     if(done_waiting_for_fresh_frame) begin
  //
  //       if (line_counter == 2'b11)
  //         line_counter = 2'b01;
  //       else
  //         line_counter = line_counter + 1;
  //
  //     end
  //
  //   end
  //
  //   else
  //     line_counter = 2'b00;
  //
  // end

  // Max 512 bytes - matching max page program size for flash
  reg reset_FIFO_bytes_written_counter = 1'b0;
  reg [11:0] FIFO_bytes_written_counter = 12'd0;

  always@(negedge camera_1_pixel_clock or posedge reset_FIFO_bytes_written_counter) begin

    if(reset_FIFO_bytes_written_counter)
      FIFO_bytes_written_counter = 0;

    else begin

        if(FIFO_write_enable)
      // if(camera_1_LV && capturing_camera_frames && done_waiting_for_fresh_frame && (line_counter == interleave_phase))
        FIFO_bytes_written_counter = FIFO_bytes_written_counter + 1;

    end

  end

  always@(posedge camera_1_LV) begin
    if(capturing_camera_frames && done_waiting_for_fresh_frame) begin
      if (line_counter == 4'd7)
        line_counter = 4'd0;
      else
        line_counter = line_counter + 1;
    end
    else
      line_counter = 4'd7;
  end

  always@(camera_1_LV) begin
    if(camera_1_LV) begin
      if(capturing_camera_frames && done_waiting_for_fresh_frame) begin

        if (line_counter == interleave_phase)
          FIFO_write_enable = 1'b1;
        else
          FIFO_write_enable = 1'b0;

      end
    end
    else
      FIFO_write_enable = 1'b0;
  end




  always@(posedge camera_1_FV or posedge reset_new_camera_FV) begin
    if(reset_new_camera_FV) begin
      new_camera_FV = 1'b0;
    end
    else begin
      if(current_state == wait_for_new_camera_FV)
        new_camera_FV = 1'b1;
    end
  end



  reg reset_PP3_command_counter = 1'b0;
  reg [3:0] PP3_command_counter = 4'd0;

  always@(posedge PP3 or posedge reset_PP3_command_counter) begin

    if(reset_PP3_command_counter)
      PP3_command_counter = 0;

    else begin

      PP3_command_counter = PP3_command_counter + 1;

    end

  end


  reg reset_PP3_command_SPI_cycle_counter = 1'b0;
  reg [9:0] PP3_command_SPI_cycle_counter = 10'd0; // Max PP3 SPI cycles = 516 = 1 instruction byte + 3 address + 512 data bytes

  always@(posedge flash_mem_interface_spi_cycle_done or posedge reset_PP3_command_SPI_cycle_counter) begin

    if(reset_PP3_command_SPI_cycle_counter)
      PP3_command_SPI_cycle_counter = 0;

    else begin

      if(current_state == flash_PP3)
        PP3_command_SPI_cycle_counter = PP3_command_SPI_cycle_counter + 1;

    end

  end







  reg reset_PP4_command_counter = 1'b0;
  reg [3:0] PP4_command_counter = 4'd0;

  always@(posedge PP4 or posedge reset_PP4_command_counter) begin

    if(reset_PP4_command_counter)
      PP4_command_counter = 0;

    else begin

      // if(current_state == flash_PP4)
      PP4_command_counter = PP4_command_counter + 1;

    end

  end


  reg reset_PP4_command_SPI_cycle_counter = 1'b0;
  reg [9:0] PP4_command_SPI_cycle_counter = 10'd0;

  always@(posedge flash_mem_interface_spi_cycle_done or posedge reset_PP4_command_SPI_cycle_counter) begin

    if(reset_PP4_command_SPI_cycle_counter)
      PP4_command_SPI_cycle_counter = 0;

    else begin

      if(current_state == flash_PP4)
        PP4_command_SPI_cycle_counter = PP4_command_SPI_cycle_counter + 1;

    end

  end


  // always@(posedge flash_mem_interface_spi_cycle_done or posedge reset_new_camera_FV) begin
  //   if(reset_new_camera_FV) begin
  //     new_camera_FV = 1'b0;
  //   end
  //   else begin
  //     if(current_state == wait_for_new_camera_FV)
  //       new_camera_FV = 1'b1;
  //   end
  // end




  // always@(negedge camera_1_pixel_clock or posedge reset_new_camera_FV) begin
  //   if(reset_new_camera_FV) begin
  //     new_camera_FV = 1'b0;
  //   end
  //   else begin
  //     if(camera_1_FV) begin
  //       if(current_state == wait_for_new_camera_FV)
  //         new_camera_FV = 1'b1;
  //     end
  //   end
  //
  // end
  //
  // // always@(posedge camera_1_LV or posedge reset_new_camera_LV) begin
  // //   if(reset_new_camera_LV) begin
  // //     new_camera_LV = 1'b0;
  // //   end
  // //   else begin
  // //     if(current_state == wait_for_new_camera_LV)
  // //       new_camera_LV = 1'b1;
  // //   end
  // // end
  //
  //
  // always@(posedge camera_1_LV or posedge reset_new_camera_LV) begin
  //   if(reset_new_camera_LV) begin
  //     new_camera_LV = 1'b0;
  //   end
  //   else begin
  //     if(current_state == wait_for_new_camera_LV)
  //       new_camera_LV = 1'b1;
  //   end
  // end








endmodule
