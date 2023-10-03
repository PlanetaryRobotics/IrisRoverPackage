
// TODO: Add exit to possible infinite WREN_RDSR1_WAIT->WREN_RDSR1->WREN_RDSR1_CHECK loop if WREN command is missed (count cycles and bump back to WREN_WAIT on issue?)
// -- seems to work relatively robustly on Earth (millions of bytes in a row) but if WREN command is missed or misinterpreted due to SEU we get stuck here forever. Want to prevent this.
// TODO: Make sure `dcfifo` is using ECC (needs 32b FIFO width and Arria 10 with M20K?)


module CRPII_FPGAFirmware_Release(

  // System Inputs
  input clk,                                // (FPGA PIN E1)

  // Hercules SPI Master
  input wire hercules_SCK,                  // SPI clock            (FPGA Pin R1)
  input wire hercules_SS,                   // Chip select          (FPGA Pin P2)
  input wire hercules_MOSI,                 // Master Out Slave In  (FPGA Pin K2) *SBC Schematic is wrong (seems to be right in Rev I)
  output wire hercules_MISO,                // Master In Slave Out  (FPGA Pin L1) *SBC Schematic is wrong (seems to be right in Rev I)

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

  // Camera select - from Hercules
  input wire camera_select,                 // Selects the camera to use (FPGA Pin L4)
  // Camera 1 Interface
  output reg  camera_1_reset_bar,           // Camera reset     (FPGA Pin B10)
  output wire camera_1_standby_bar,         // Standby          (FPGA Pin E11)
  output wire camera_1_trigger,             // Trigger          (FPGA Pin A11)
  input wire camera_1_pixel_clock,          // Pixel clock      (FPGA Pin D9)
  input wire camera_1_FV,                   // Frame valid flag (FPGA Pin C9)
  input wire camera_1_LV,                   // Line valid flag  (FPGA Pin A10)
  input wire [11:0] camera_1_pixel_in,      // Pixel data

  // Camera 2 Interface
  output reg  camera_2_reset_bar,           // Camera reset     (FPGA Pin R3)
  output wire camera_2_standby_bar,         // Standby          (FPGA Pin P3)
  output wire camera_2_trigger,             // Trigger          (FPGA Pin T2)
  input wire camera_2_pixel_clock,          // Pixel clock      (FPGA Pin T4)
  input wire camera_2_FV,                   // Frame valid flag (FPGA Pin R4)
  input wire camera_2_LV,                   // Line valid flag  (FPGA Pin T3)
  input wire [11:0] camera_2_pixel_in,      // Pixel data
  // LEDs
  output wire LED0,                         // Red LED    (FPGA Pin L2)
  output reg LED1,                          // Green LED  (FPGA Pin N1)
  output reg LED2                           // Blue LED   (FPGA Pin K1)

  // Debug
  // output wire camera_2_port_pin_2,           // (FPGA Pin M8)
  // output wire camera_2_port_pin_4,           // (FPGA Pin T7)
  // output wire camera_2_port_pin_8,           // (FPGA Pin N6)
  // output wire camera_2_port_pin_10           // (FPGA Pin R6)
);



  // Debug
  // assign camera_2_port_pin_2 = camera_1_pixel_clock;
  // assign camera_2_port_pin_4 = flash_CS1;
  // assign camera_2_port_pin_8 = FIFO_write_enable;
  // assign camera_2_port_pin_10 = camera_1_LV;

  // assign camera_2_port_pin_2 = camera_1_FV;
  // assign camera_2_port_pin_4 = flash_CS1;
  // assign camera_2_port_pin_8 = camera_1_LV;
  // assign camera_2_port_pin_10 = new_camera_FV;

  // assign camera_2_port_pin_8 = pll_is_locked;
  // assign camera_2_port_pin_10 = pll_clock;

  // Ensure cameras are not in standby:
  assign camera_1_standby_bar = 1'b1;
  assign camera_2_standby_bar = 1'b1;

  // Connect to Camera Interface:
  wire request_image = 1'b0; // input to `CameraSensorInterface`
  wire image_started;        // output from `CameraSensorInterface`
  wire image_finished;       // output from `CameraSensorInterface`
  wire [7:0] data_for_fifo;  // output from `CameraSensorInterface`
  wire sample_pixel_pulse;   // output from `CameraSensorInterface`
  wire keep_pixel;           // output from `CameraSensorInterface`
  CameraSensorInterface cameraInterface(
    .clk(clk),
    // Signals from camera sensors:
    .camera_1_reset_bar   (camera_1_reset_bar),
    .camera_1_pixel_clock (camera_1_pixel_clock),
    .camera_1_FV          (camera_1_FV),
    .camera_1_LV          (camera_1_LV),
    .camera_1_pixel_in    (camera_1_pixel_in),
    .camera_2_reset_bar   (camera_2_reset_bar),
    .camera_2_pixel_clock (camera_2_pixel_clock),
    .camera_2_FV          (camera_2_FV),
    .camera_2_LV          (camera_2_LV),
    .camera_2_pixel_in    (camera_2_pixel_in),
    // Context information:
    .desired_camera_idx(~camera_select),  // Placeholder until WD FSM comes online
    .imaging_mode      (~camera_select),  // Placeholder until WD FSM comes online
    // I/O control & status signals:
    .request_image(request_image),
    .image_started(image_started),
    .image_finished(image_finished),
    // FIFO Interface:
    .data_for_fifo(data_for_fifo),
    .sample_pixel_pulse(sample_pixel_pulse),
    .keep_pixel(keep_pixel)
  );


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
  reg [7:0] hercules_take_picture_command = 8'hFF;

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



  // Resets initialized to active reset condition
  reg flash_interface_reset = 1'b0;

  wire flash_mem_interface_done;
  wire flash_mem_interface_spi_cycle_done;
  wire flash_mem_interface_output_valid;

  reg READ_ID = 1'b0;
  reg READ3 = 1'b0;
  reg WREN = 1'b0;
  reg RDSR1 = 1'b0;
  reg PP3 = 1'b0;
  reg PP4 = 1'b0;

  reg [31:0] address = 32'd0;
  wire [7:0] output_data;
  reg [31:0] num_bytes = 32'd0;
  reg [9:0] PP3_send_bytes_counter__current_page;

  wire debug_out;

  S70FL01GS_Flash_Memory_Interface_address_swap flash_mem(
  // S70FL01GS_Flash_Memory_Interface flash_mem(

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
    .PP3_send_bytes_counter(PP3_send_bytes_counter__current_page),

    .input_data(byte_to_flash),
    .output_data(output_data),

    .debug_out(debug_out)
  );


  // Flash FSM States

  reg [4:0] current_state, next_state;

  parameter reset_state                             = 5'd0;
  parameter idle                                    = 5'd1;
  parameter flash_RDSR1_wait                        = 5'd9;
  parameter flash_RDSR1                             = 5'd10;
  parameter flash_RDSR1_check_WIP                   = 5'd22;
  parameter wait_for_start_of_image_streaming       = 5'd18;
  parameter flash_WREN_wait                         = 5'd4;
  parameter flash_WREN                              = 5'd5;
  parameter flash_WREN_RDSR1_wait                   = 5'd6;
  parameter flash_WREN_RDSR1                        = 5'd7;
  parameter flash_WREN_RDSR1_check                  = 5'd8;
  parameter flash_PP3_wait                          = 5'd13;
  parameter flash_PP3                               = 5'd14;
  parameter flash_PP3_RDSR1_wait                    = 5'd15;
  parameter flash_PP3_RDSR1                         = 5'd16;
  parameter flash_PP3_RDSR1_check                   = 5'd17;
  parameter check_image_status                      = 5'd31;


  reg [23:0] powerup_reset_counter = 24'd0;

  reg [31:0] wait_counter = 32'd0;

  reg flash_WREN_done = 1'b0;
  reg reset_flash_WREN_done = 1'b0;

  reg flash_WREN_RDSR1_done = 1'b0;
  reg reset_flash_WREN_RDSR1_done = 1'b0;
  reg [7:0] flash_WREN_RDSR1_response = 8'd0;

  reg flash_RDSR1_done = 1'b0;
  reg reset_flash_RDSR1_done = 1'b0;
  reg [7:0] flash_RDSR1_response = 8'd0;

  reg flash_PP3_done = 1'b0;
  reg reset_flash_PP3_done = 1'b0;

  reg flash_PP3_RDSR1_done = 1'b0;
  reg reset_flash_PP3_RDSR1_done = 1'b0;
  reg [7:0] flash_PP3_RDSR1_response = 8'd0;

  // Flash State Machine:
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

        end

        // Reset
        else begin
          // Increment counter
          powerup_reset_counter = powerup_reset_counter + 1;

          // Hold Hercules SPI listener interface in reset
          hercules_SPI_listener_reset = 1'b0;

          // Hold flash interface in reset
          flash_interface_reset = 1'b0;
        end

      end


      // IDLE
      idle: begin

        // // Wait time
        // if (wait_counter == 32'h00100000) begin
        //   next_state = flash_RDSR1_wait;
        //   wait_counter = 0;
        // end
        // else begin
        //   wait_counter = wait_counter + 1;
        // end

        // reset_flash_WREN_done = 1'b0;

        // Previous state reset of counter reset
        reset_PP3_command_counter = 1'b0;

        // Wait for take picture trigger
        if(hercules_SPI_listener_rx_data == hercules_take_picture_command) begin
          next_state = flash_RDSR1_wait;

          // FPGA takes over flash memory control
          flash_SPI_controller = FPGA_controls_flash;

          // Hold Hercules SPI listener interface in reset
          hercules_SPI_listener_reset = 1'b0;

        end

        // Otherwise, hercules controls flash memory
        else begin

          // Hercules holds flash memory control
          flash_SPI_controller = hercules_controls_flash;

          // Take Hercules SPI listener interface out of reset
          hercules_SPI_listener_reset = 1'b1;
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
          next_state = wait_for_start_of_image_streaming;
        else
          next_state = flash_RDSR1_wait;
      end


      // We need to wait for a FV assertion to get the start of an image
      wait_for_start_of_image_streaming: begin
        // Waiting for the CameraSensorInterface to start streaming bytes into
        // the FIFO:
        if (image_started) begin
          next_state = flash_WREN_wait;
          
          // Image start address in flash
          address = 32'h00000000;
        end
      end


      flash_WREN_wait: begin
        // Previous state reset of counter reset
        // (when we come here from `check_image_status`):
        reset_PP3_command_counter = 1'b0;

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


      // Make sure we have enough data in the FIFO before proceeding:
      flash_PP3_wait: begin
        // Set the number of bytes we need to write for this page:
        // Lines are 2592B long, so we write in 2592B sections to flash.
        // That means we write 5 512B pages (max size) then one 32B page to
        // make up the difference. Afterwards, we advance to the start of the
        // next page so every 6 pages contains 1 line:
        num_bytes = (PP3_command_counter == 5) ? 32'd32 : 32'd512;

        if(FIFO_is_empty) begin // FIFO IP requires us to check this in addition to rdusedw as a safety no matter what
          // As a safety, if it looks like we're totally done, jump to waiting
          // to make sure all writes are complete:
          if(image_finished) begin
            next_state = flash_PP3_RDSR1_wait;
            // Also perform the same transition actions as PP3 performs when
            // making this transition (just in case):
            reset_flash_PP3_done = 1'b1;
            PP3 = 1'b0;
          end
        end else begin // ~FIFO_is_empty
          // Only advance to writing this page if there's enough data in the FIFO:
          if(
            (FIFO_read_bytes_avail >= num_bytes) // enough data in the FIFO for this page
            || image_finished // as a safety, if the image is done, then we just need to drain the FIFO of everything it's got (helps us handle if a single byte got missed)
          ) begin
            next_state = flash_PP3;
          end
        end
        // ^ doing this as one check tree instead of two `FIFO_is_empty && X`
        // checks to prevent race condition where `FIFO_is_empty` updates
        // between checks
      end


      // Execute page program:
      flash_PP3: begin
        if(flash_PP3_done) begin
          reset_flash_PP3_done = 1'b1;
          PP3 = 1'b0;
          next_state = flash_PP3_RDSR1_wait;
        end
        else begin
          // Hold PP3 write flag high until we're done with the transaction:
          PP3 = 1'b1;
        end
      end


      flash_PP3_RDSR1_wait: begin
        // Deassert previous state's done reset flag
        reset_flash_PP3_done = 1'b0;

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
          next_state = check_image_status;
        else
          next_state = flash_PP3_RDSR1_wait;
      end


      check_image_status: begin
        if(
          image_finished    // Camera interface is done adding bytes to the FIFO
          && FIFO_is_empty  // we've written everything in the FIFO to flash
        ) begin
          // We're done:
          next_state = idle;
        end else begin
          // Move on to the start of the next page
          // (no matter what, we always do this - we stride in pages)
          address = address + 32'h00000200;
          // Start the page writing process over (get write lock then write):
          next_state = flash_WREN_wait;

          // See if we need to reset anything:
          if(PP3_command_counter == 6) begin
            // Just finished all the pages in a "line" (2592B section),
            // (5 512B pages + 32B in the 6th page)
            // reset the counter:
            reset_PP3_command_counter = 1'b1;
          end
        end
      end


      default: begin
        next_state = reset_state;
      end

    endcase

  end


  // * State transition @ NEGEDGE clk
  always@(negedge clk) begin
      current_state <= next_state;
  end


  // * WREN Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_WREN_done) begin
    if(reset_flash_WREN_done) begin
      flash_WREN_done = 1'b0;
    end
    else begin
      if(current_state == flash_WREN)
        flash_WREN_done = 1'b1;
    end
  end


  // * WREN RDSR1 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_WREN_RDSR1_done) begin
    if(reset_flash_WREN_RDSR1_done) begin
      flash_WREN_RDSR1_done = 1'b0;
    end
    else begin
      if(current_state == flash_WREN_RDSR1)
        flash_WREN_RDSR1_done = 1'b1;
    end
  end


  // * RDSR1 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_RDSR1_done) begin
    if(reset_flash_RDSR1_done) begin
      flash_RDSR1_done = 1'b0;
    end
    else begin
      if(current_state == flash_RDSR1)
        flash_RDSR1_done = 1'b1;
    end
  end


  // * PP3 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_PP3_done) begin
    if(reset_flash_PP3_done) begin
      flash_PP3_done = 1'b0;
    end
    else begin
      if(current_state == flash_PP3)
        flash_PP3_done = 1'b1;
    end
  end


  // * PP3 RDSR1 Done
  always@(posedge flash_mem_interface_done or posedge reset_flash_PP3_RDSR1_done) begin
    if(reset_flash_PP3_RDSR1_done) begin
      flash_PP3_RDSR1_done = 1'b0;
    end
    else begin
      if(current_state == flash_PP3_RDSR1)
        flash_PP3_RDSR1_done = 1'b1;
    end
  end


  // * PP3 Command Counter:
  reg reset_PP3_command_counter = 1'b0;
  reg [3:0] PP3_command_counter = 4'd0;
  always@(posedge PP3 or posedge reset_PP3_command_counter) begin
    if(reset_PP3_command_counter)
      PP3_command_counter = 0;
    else begin
      PP3_command_counter = PP3_command_counter + 1;
    end
  end


  // * FIFO Read Acknowledge:
  // 1 sys-clk pulse telling FIFO that PP3 just finished writing another byte.
  // Tell FIFO this so show-ahead can bring forward the next byte ASAP (delay
  // is 1 rdclk).
  reg [9:0] PP3_send_bytes_counter__current_page__prev_clk = 1'b0;
  wire PP3_spi_cycle_completed_pulse = flash_mem_interface_spi_cycle_done & ~flash_mem_interface_spi_cycle_done__prev_clk;  // 1 sys-clk pulse saying we just completed a SPI cycle
  wire FIFO_read_ack = PP3_spi_cycle_completed_pulse;
  always @(posedge clk) begin
    // Keep track of state change every sys-clk:
    PP3_send_bytes_counter__current_page__prev_clk <= PP3_send_bytes_counter__current_page;
  end



  // * Camera pixel FIFO
  wire [7:0] byte_to_flash;  // byte to write to Flash from FIFO
  wire request_image = 1'b0; // input to `CameraSensorInterface`
  wire image_started;        // output from `CameraSensorInterface`
  wire image_finished;       // output from `CameraSensorInterface`
  wire [7:0] data_for_fifo;  // output from `CameraSensorInterface`
  wire sample_pixel_pulse;   // output from `CameraSensorInterface`
  wire keep_pixel;           // output from `CameraSensorInterface`
  // FIFO Control & Response:
  wire FIFO_is_empty;
  wire FIFO_is_full;
  reg FIFO_read_ack = 1'b0;  // Acknowledge that Flash has read the current byte and show-ahead should present the next one.
  // Number of bytes available on the read side of the FIFO:
  reg [15:0] FIFO_read_bytes_avail;

  FIFO CAM_FIFO
  (
    	.data( data_for_fifo[7:0] ),
      .wrclk( clk ),
    	.wrreq( keep_pixel & sample_pixel_pulse ),
    	.wrfull( FIFO_is_full ),

      .rdclk( clk ),
      .rdreq( FIFO_read_ack ),
    	.q( byte_to_flash ),
    
    	.rdempty( FIFO_is_empty ),
      .rdusedw(FIFO_read_bytes_avail)
  );

endmodule
