
// TODO: Add exit to possible infinite WREN_RDSR1_WAIT->WREN_RDSR1->WREN_RDSR1_CHECK loop if WREN command is missed (count cycles and bump back to WREN_WAIT on issue?)
// -- seems to work relatively robustly on Earth (millions of bytes in a row) but if WREN command is missed or misinterpreted due to SEU we get stuck here forever. Want to prevent this.
// TODO: Make sure `dcfifo` is using ECC (needs 32b FIFO width and Arria 10?)


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



  wire FIFO_is_empty;
  wire FIFO_is_full;
  reg FIFO_read_req = 1'b0;
  reg FIFO_write_enable = 1'b0;

  reg capturing_camera_frames = 1'b0;

  reg waiting_for_fresh_frame = 1'b0; // Appears to be unused
  reg done_waiting_for_fresh_frame = 1'b0;



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

  // Ensure camera is not in standby
  assign camera_1_standby_bar = 1'b1;
  assign camera_2_standby_bar = 1'b1;



  wire selected_camera_pixel_clock;
  wire selected_camera_FV;
  wire selected_camera_LV;
  wire [11:0] selected_camera_pixel_in;

  // Mux for selecting the correct camera inputs
  //    Camera 1 - camera_select = 1
  //    Camera 2 - camera_select = 0

  // Deprecated & Disconnected:
  assign selected_camera_pixel_clock    = camera_select ? camera_1_pixel_clock  : camera_2_pixel_clock;
  assign selected_camera_FV             = camera_select ? camera_1_FV           : camera_2_FV;
  assign selected_camera_LV             = camera_select ? camera_1_LV           : camera_2_LV;
  assign selected_camera_pixel_in       = camera_select ? camera_1_pixel_in     : camera_2_pixel_in;

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
  // reg [7:0] input_data;
  wire [7:0] output_data;
  reg [31:0] num_bytes = 32'd0;
  reg [9:0] PP3_send_bytes_counter__current_page;

  reg [7:0] latest_output_data;

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




  // FSM States

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
  parameter flash_PP3_repeat_for_next_page_or_exit  = 5'd23;
  parameter flash_PP3_repeat_for_next_line_or_exit  = 5'd30;
  parameter flash_PP3_repeat_for_next_frame_or_exit = 5'd31;
  // Deprecated states:
  parameter flash_READ_ID_wait                      = 5'd2;
  parameter flash_READ_ID                           = 5'd3;
  parameter flash_READ3                             = 5'd12;
  parameter flash_READ3_wait                        = 5'd11;
  parameter wait_for_new_camera_LV                  = 5'd19;
  parameter wait_to_switchover_flash_control        = 5'd20;
  parameter flash_PP3_check_FIFO                    = 5'd21;
  parameter flash_PP4_wait                          = 5'd24;
  parameter flash_PP4                               = 5'd25;
  parameter flash_PP4_RDSR1_wait                    = 5'd26;
  parameter flash_PP4_RDSR1                         = 5'd27;
  parameter flash_PP4_RDSR1_check                   = 5'd28;
  parameter flash_PP4_repeat_or_exit                = 5'd29;


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

  reg PP3_wrote_one_line = 1'b0;

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

        reset_lines_of_interleave_phase = 1'b0;

        // Wait for take picture trigger
        if(hercules_SPI_listener_rx_data == hercules_take_picture_command) begin
          next_state = flash_RDSR1_wait;

          // FPGA takes over flash memory control
          flash_SPI_controller = FPGA_controls_flash;

          // Hold Hercules SPI listener interface in reset
          hercules_SPI_listener_reset = 1'b0;

          // keep this 0 always since we're deprecating the old camera control circuit:
          capturing_camera_frames = 1'b0;

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
          next_state = wait_for_start_of_image_streaming;
        else
          next_state = flash_RDSR1_wait;
      end


      // We need to wait for a FV assertion to get the start of an image
      wait_for_start_of_image_streaming: begin
        // Waiting for the CameraSensorInterface to start streaming bytes into
        // the FIFO:
        reset_lines_of_interleave_phase = 1'b0;

        if (image_started) begin
          next_state = flash_WREN_wait;
          
          // Image start address in flash
          address = 32'h00000000;
        end
      end


      flash_WREN_wait: begin
        LED1 = 1'b1; // Off

        // Deassert previous state's done reset flag
        // reset_flash_READ3_done = 1'b0;
        // reset_flash_READ_ID_done = 1'b0;

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

        // if(FIFO_bytes_written_counter > 12'd0)
        //   LED2 = 1'b0; // On
        // else
        //   LED2 = 1'b1; // Off

        // Wait for full FIFO
        if(PP3_command_counter == 0) begin
          if (FIFO_bytes_written_counter > 12'd1) begin
          // if (!FIFO_is_empty) begin

            next_state = flash_PP3;
            num_bytes = 32'd512;

            // // Image start address
            // address = 32'h00000000;

          end
        end

        else begin
            next_state = flash_PP3;

            if(PP3_command_counter == 5)
                num_bytes = 32'd32;
            else
                num_bytes = 32'd512;

        end
      end


      // Trigger page program
      flash_PP3: begin
        // Deassert previous state's counter reset flag
        // reset_FIFO_bytes_written_counter = 1'b0;

        // LED2 = 1'b1; // Off
        // LED2 = 1'b0; // On

        // ! TODO: (WORKING-HERE): Integrate `PP3_send_bytes_counter__current_page` and continue remodel from here in the flow.
        if(flash_PP3_done) begin
          reset_flash_PP3_done = 1'b1;
          reset_PP3_command_SPI_cycle_counter = 1'b1;
          reset_PP3_FIFO_read_req = 1'b1;

          PP3 = 1'b0;
          // address = address + 32'h00000200;
          // num_bytes = 32'd0;

          // FIFO_read_req = 1'b0;
          // input_data = 8'h11;

          next_state = flash_PP3_RDSR1_wait;
        end
        else begin
          // Hold PP3 write flag high until we're done with the transaction:
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

        // Deassert previous state's SPI cycle counter reset flag
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
          next_state = flash_PP3_repeat_for_next_page_or_exit;
        else
          next_state = flash_PP3_RDSR1_wait;
      end


      // // Write data to flash from FIFO as long as its not empty
      // flash_PP3_check_FIFO: begin
      //
      //   if(FIFO_is_empty) begin
      //     next_state = idle;
      //
      //   end
      //   else
      //     next_state = flash_WREN_wait;
      //
      // end


      // Repeating page writes to complete line or finish
      flash_PP3_repeat_for_next_page_or_exit: begin

        if(PP3_command_counter == 6) begin
        // if(PP3_command_counter == 2) begin
          next_state = flash_PP3_repeat_for_next_line_or_exit;

          reset_PP3_command_counter = 1'b1;
          reset_FIFO_bytes_written_counter = 1'b1;

          PP3_wrote_one_line = 1'b1;
          // address = address + 32'h00000200;
        end

        // Otherwise we need another page write to flash
        else begin
          next_state = flash_WREN_wait;

          // Increment 512 bytes, the max length of PP3 data
          address = address + 32'h00000200;
        end

      end


      // Repeating writes of every 8th line
      flash_PP3_repeat_for_next_line_or_exit: begin

        reset_PP3_command_counter = 1'b0;
        reset_FIFO_bytes_written_counter = 1'b0;

        PP3_wrote_one_line = 1'b0;

        // Reading every 8th line of a frame results in 1944 / 8 = 243 lines per frame
        //  Line counter auto-resets at negedge of FV
        if(lines_of_interleave_phase == 8'd243) begin
          next_state = flash_PP3_repeat_for_next_frame_or_exit;

          reset_lines_of_interleave_phase = 1'b1;
        end

        // Otherwise we need another line for this interleaving phase
        else begin
          next_state = flash_WREN_wait;

          // Every line requires 6 pages of space, and reading every 8th line
          //  requires the address to be offset
          //  - Subtract 5 pages from address to return to line start address
          //  - Add 8 lines to the line start address to find the next 8th line
          // address = address - 32'h0000A00 + 32'h00006000;
          // address = address + 32'h0000200;
          //
          // ^ THIS assumes we handle interleaving here. Seems it was decided
          // to just stream data into the flash and let Ground handle
          // reconstruction. More versatile anyway.
          // Instead, just keep writing more pages:
          address = address + 32'h00000200;
        end

      end


      // Repeating writes to interleave 8 images
      flash_PP3_repeat_for_next_frame_or_exit: begin

        interleave_phase = interleave_phase + 1;

        reset_lines_of_interleave_phase = 1'b0;

        // Reading every 8th line of a frame results in 1944 / 8 = 243 lines per frame
        //  Line counter auto-resets at negedge of FV
        if(interleave_phase == phase_7) begin
        // if(interleave_phase == phase_0) begin
          next_state = idle;
          interleave_phase = phase_0;
        end

        // Otherwise we need another line for this interleaving phase
        else begin
          next_state = flash_WREN_wait;

          // interleave_phase = interleave_phase + 1;

          address = address + 32'h00000200;
        end

      end


      //################
      // UNUSED STATES


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


  // // FIFO control and PP3 input data
  // always@(posedge flash_mem_interface_spi_cycle_done or posedge reset_PP3_FIFO_read_req) begin
  //
  //   if(reset_PP3_FIFO_read_req)
  //     FIFO_read_req = 1'b0;
  //
  //   else begin
  //     if(current_state == flash_PP3 && PP3_command_SPI_cycle_counter >= 4) begin
  //       FIFO_read_req = 1'b1;
  //       input_data = byte_to_flash;
  //     end
  //   end
  //
  // end


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



//* CAMERA CONTROL LOGIC:


  parameter phase_0 = 4'd0;
  parameter phase_1 = 4'd1;
  parameter phase_2 = 4'd2;
  parameter phase_3 = 4'd3;
  parameter phase_4 = 4'd4;
  parameter phase_5 = 4'd5;
  parameter phase_6 = 4'd6;
  parameter phase_7 = 4'd7;


  reg [3:0] interleave_phase = phase_0;

  reg [3:0] line_counter = 4'd0;

  reg [7:0] lines_of_frame_counter = 8'd0;

  reg [7:0] lines_of_interleave_phase = 8'd0;
  reg reset_lines_of_interleave_phase = 1'b0;

  wire [7:0] byte_to_flash;  // byte to write to Flash from FIFO

  // Camera pixel FIFO

  // assign pixel_is_valid = ! FIFO_is_empty && rx_Valid_out;
  // assign FIFO_read_req = ! FIFO_is_empty && rx_Valid_out ;

  wire request_image = 1'b0; // input to `CameraSensorInterface`
  wire image_started;        // output from `CameraSensorInterface`
  wire image_finished;       // output from `CameraSensorInterface`
  wire [7:0] data_for_fifo;  // output from `CameraSensorInterface`
  wire sample_pixel_pulse;   // output from `CameraSensorInterface`
  wire keep_pixel;           // output from `CameraSensorInterface`
  reg FIFO_read_ack = 1'b0;         // Acknowledge that Flash has read the current byte and show-ahead should present the next one.
  FIFO CAM_FIFO
  (
    	.data( data_for_fifo[7:0] ),
      .wrclk( sample_pixel_pulse ),
    	.wrreq( keep_pixel ),

      .rdclk( flash_mem_interface_spi_cycle_done ),
      .rdreq( FIFO_read_ack ),
    	.q( byte_to_flash ),

    	.rdempty( FIFO_is_empty ),
    	.wrfull( FIFO_is_full )
  );


  // * DEPRECATED (& disconnected) CAMERA LOGIC *:


  //* FRAME LINE COUNTER:
  always@(posedge selected_camera_LV or negedge selected_camera_FV) begin

    if(!selected_camera_FV) begin
      lines_of_frame_counter = 0;
    end
    else
      lines_of_frame_counter = lines_of_frame_counter + 1;

  end



  always@(posedge PP3_wrote_one_line or posedge reset_lines_of_interleave_phase) begin

    if(reset_lines_of_interleave_phase) begin
      lines_of_interleave_phase = 0;
    end
    else
      lines_of_interleave_phase = lines_of_interleave_phase + 1;

  end



  // Max 512 bytes - matching max page program size for flash
  reg reset_FIFO_bytes_written_counter = 1'b0;
  reg [11:0] FIFO_bytes_written_counter = 12'd0;

  always@(negedge selected_camera_pixel_clock or posedge reset_FIFO_bytes_written_counter) begin

    if(reset_FIFO_bytes_written_counter)
      FIFO_bytes_written_counter = 0;

    else begin
        if(FIFO_write_enable)
      // if(selected_camera_LV && capturing_camera_frames && done_waiting_for_fresh_frame && (line_counter == interleave_phase))
        FIFO_bytes_written_counter = FIFO_bytes_written_counter + 1;
    end

  end



  // NOTE: `line_counter` appears to be unused in favor of `lines_of_frame_counter`
  // ...also this only counts to 7... seems like it was used for interleave
  always@(posedge selected_camera_LV) begin
    if(capturing_camera_frames && done_waiting_for_fresh_frame) begin
      if (line_counter == 4'd7)
        line_counter = 4'd0;
      else
        line_counter = line_counter + 1;
    end
    else
      line_counter = 4'd0;
  end



  always@(selected_camera_LV) begin
    if(selected_camera_LV) begin
      if(capturing_camera_frames && done_waiting_for_fresh_frame) begin

        // if (line_counter == interleave_phase)
        if ((lines_of_frame_counter - interleave_phase - 1) % 8 == 0)
          FIFO_write_enable = 1'b1;
        else
          FIFO_write_enable = 1'b0;

      end
    end
    else
      FIFO_write_enable = 1'b0;
  end



  always@(posedge selected_camera_FV or posedge reset_new_camera_FV) begin
    if(reset_new_camera_FV) begin
      new_camera_FV = 1'b0;
    end
    else begin
      if(current_state == wait_for_start_of_image_streaming)
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



  // * FIFO Read Acknowledge 1 sys-clk pulse telling FIFO we read the current
  // byte and it should present the next one (show-ahead mode).
  reg reset_PP3_command_SPI_cycle_counter = 1'b0;
  reg [9:0] PP3_command_SPI_cycle_counter = 10'd0; // Max PP3 SPI cycles = 516 = 1 instruction byte + 3 address + 512 data bytes
  reg flash_mem_interface_spi_cycle_done__prev_clk = 1'b0;
  wire PP3_spi_cycle_completed_pulse = flash_mem_interface_spi_cycle_done & ~flash_mem_interface_spi_cycle_done__prev_clk;  // 1 sys-clk pulse saying we just completed a SPI cycle
  always @(posedge clk) begin
    flash_mem_interface_spi_cycle_done__prev_clk <= flash_mem_interface_spi_cycle_done;

    if(reset_PP3_command_SPI_cycle_counter)
      PP3_command_SPI_cycle_counter <= 0;
    else if( (current_state == flash_PP3) && PP3_spi_cycle_completed_pulse ) begin
      PP3_command_SPI_cycle_counter <= PP3_command_SPI_cycle_counter + 1;

      FIFO_read_ack
    end

    if() begin
      // SPI Cycle just ended
    end
    
  end




  // Old PP3 FIFO Control:
  // // FIFO control and PP3 input data

  // always@(posedge flash_mem_interface_spi_cycle_done or posedge reset_PP3_FIFO_read_req) begin

  //   if(reset_PP3_FIFO_read_req)
  //     FIFO_read_req = 1'b0;
  //   else begin
  //     if(current_state == flash_PP3 && PP3_command_SPI_cycle_counter >= 3) begin
  //       FIFO_read_req = 1'b1;
  //       input_data = byte_to_flash;
  //     end
  //   end

  // end








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
  //     if(current_state == wait_for_start_of_image_streaming)
  //       new_camera_FV = 1'b1;
  //   end
  // end


  // always@(negedge camera_1_pixel_clock or posedge reset_new_camera_FV) begin
  //   if(reset_new_camera_FV) begin
  //     new_camera_FV = 1'b0;
  //   end
  //   else begin
  //     if(camera_1_FV) begin
  //       if(current_state == wait_for_start_of_image_streaming)
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
