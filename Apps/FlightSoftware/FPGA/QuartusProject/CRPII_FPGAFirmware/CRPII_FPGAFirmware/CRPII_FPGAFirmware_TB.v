// NOTE
// Simulation must be run with the altera_mf_ver library to simulate PLL
//  vsim -L altera_mf_ver work.CRPII_FPGAFirmware_TB

`timescale 1ns / 1ps

module CRPII_FPGAFirmware_TB();


// System Inputs
reg clk;


// To Flash Memory (S70FL01GS) - SPI Slave
wire flash_reset;
wire flash_SCK;
wire flash_CS1;
wire flash_CS2;
wire flash_SI;
wire flash_SO;
wire flash_WP;
wire flash_HOLD;

wire LED0;
wire LED1;
wire LED2;


// Hercules SPI Slave Interface
wire SCK; // SPI clock
wire SS; // Chip select
wire MOSI; // Master Out Slave In
wire MISO; // Master In Slave Out




// Camera 0 Interface
wire camera_1_pixel_clock; // Camera clock
reg camera_1_FV = 1'b0; // Frame valid flag
reg camera_1_LV = 1'b0; // Line valid flag
reg camera_1_strobe = 1'b0; // Strobe signal for flash control - Not needed
reg [11:0] camera_1_pixel_in = 12'h000; // Pixel data
wire camera_reset_bar;
wire camera_standby_bar;
wire camera_trigger;


CRPII_FPGAFirmware_v2 FLASH_TESTER(

  .clk(clk),

  .hercules_SCK( SCK ),
  .hercules_SS( SS ),
  .hercules_MOSI( MOSI ),
  .hercules_MISO( MISO ),

  .flash_reset(flash_reset),
  .flash_SCK(flash_SCK),
  .flash_CS1(flash_CS1),
  .flash_CS2(flash_CS2),
  .flash_SI(flash_SI),
  .flash_SO(flash_SO),
  .flash_WP(flash_WP),
  .flash_HOLD(flash_HOLD),

  .LED0(LED0),
  .LED1(LED1),
  .LED2(LED2),

  // Camera 1 Interface
  .camera_1_FV( camera_1_FV ),
  .camera_1_LV( camera_1_LV ),
  .camera_1_pixel_in( camera_1_pixel_in ),
  .camera_1_pixel_clock( camera_1_pixel_clock ),

  .camera_1_reset_bar( camera_reset_bar ),
  .camera_1_standby_bar( camera_standby_bar ),
  .camera_1_trigger( camera_trigger )
);



// Keep testbench time
parameter PERIOD = 20; // ~50MHz clock - 20ns period
parameter real DUTY_CYCLE = 0.5;
initial begin  // Clock process for clk
    repeat(5000)
    begin
        clk = 1'b0;
        #(PERIOD-(PERIOD*DUTY_CYCLE)) clk = 1'b1;
        #(PERIOD*DUTY_CYCLE);
        clk = 1'b0;
        #(PERIOD-(PERIOD*DUTY_CYCLE)) clk = 1'b1;
        #(PERIOD*DUTY_CYCLE);
        clk = 1'b0;
        #(PERIOD-(PERIOD*DUTY_CYCLE)) clk = 1'b1;
        #(PERIOD*DUTY_CYCLE);
        clk = 1'b0;
        #(PERIOD-(PERIOD*DUTY_CYCLE)) clk = 1'b1;
        #(PERIOD*DUTY_CYCLE);
        clk = 1'b0;
        #(PERIOD-(PERIOD*DUTY_CYCLE)) clk = 1'b1;
        #(PERIOD*DUTY_CYCLE);
        clk = 1'b0;
        #(PERIOD-(PERIOD*DUTY_CYCLE)) clk = 1'b1;
        #(PERIOD*DUTY_CYCLE);
        clk = 1'b0;
        #(PERIOD-(PERIOD*DUTY_CYCLE)) clk = 1'b1;
        #(PERIOD*DUTY_CYCLE);
    end
end



reg [7:0] hercules_spi_master_command = 8'b11111111;

wire [7:0] hercules_spi_slave_response;

reg hercules_spi_master_enable = 1'b0;
wire hercules_spi_master_busy;
wire hercules_spi_master_done;
reg hercules_spi_master_reset = 1'b0;
reg multiple_byte_transfer = 1'b0;


spi_master MAS (
    .reset(hercules_spi_master_reset),
    .clk(SPI_master_clock),
    .mlb(1'b1), // Always left shift, always MSB first
    .start_SPI_transfer(hercules_spi_master_enable),
    .multiple_byte_transfer(multiple_byte_transfer),
    .transmit_data(hercules_spi_master_command),
    .clk_to_sck_divisor(2'b00), // We want fastest SCK speed available
    .MISO(MISO),
    .SS(SS),
    .SCK(SCK),
    .MOSI(MOSI),
    .done(hercules_spi_master_done),
    .received_data(hercules_spi_slave_response)
);


reg reset = 1'b0;
reg ten = 1'b0;
reg [7:0] slave_transmission_data = 8'b00000000;
wire SLVdone;
wire [7:0] SLVrdata;

spi_slave SLV (
      .reset(reset),
      .ten(1'b1),
      .tdata(slave_transmission_data),
      .mlb(1'b1),
      .SS(flash_CS1),
      .SCK(flash_SCK),
      .MOSI(flash_SI),
      .MISO(flash_SO),
      .done(SLVdone),
      .rdata(SLVrdata));


// Be sure to terminate the simulation
initial begin
  #10 reset = 1'b0;
  #100 reset = 1'b1;

  #1000;

  hercules_spi_master_command = 8'hFE;
  hercules_spi_master_enable = 1'b1;

  @(posedge hercules_spi_master_done);

  hercules_spi_master_enable = 1'b0;

  #100000 $stop;
end


// Pixel clock, FV, LV from camera stimulus
reg pixel_clock = 1'b0;
reg [2:0] line_count = 3'b000;

parameter PIXEL_CLOCK_PERIOD = 166; // ~6MHz clock - 167000ps period
parameter real PIXEL_CLOCK_DUTY_CYCLE = 0.5;

initial begin
  repeat(5000) begin
    pixel_clock = 1'b0;
    #(PIXEL_CLOCK_PERIOD-(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE));
    pixel_clock = 1'b1;
    #(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE);

    pixel_clock = 1'b0;
    #(PIXEL_CLOCK_PERIOD-(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE));
    pixel_clock = 1'b1;
    #(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE);

    pixel_clock = 1'b0;
    #(PIXEL_CLOCK_PERIOD-(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE));
    pixel_clock = 1'b1;
    #(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE);

    pixel_clock = 1'b0;
    #(PIXEL_CLOCK_PERIOD-(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE));
    pixel_clock = 1'b1;
    #(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE);

    pixel_clock = 1'b0;
    #(PIXEL_CLOCK_PERIOD-(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE));
    pixel_clock = 1'b1;
    #(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE);
  end

end


initial begin // Clock process for clk

  // // Pixel clock with no image data streaming out
  // repeat(100) begin
  //   pixel_clock = 1'b0;
  //   #(PIXEL_CLOCK_PERIOD-(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE));
  //   pixel_clock = 1'b1;
  //   #(PIXEL_CLOCK_PERIOD*PIXEL_CLOCK_DUTY_CYCLE);
  // end


  // Repeat for 20 simulated images
  repeat(20) begin

    // 15.3 - 15.4 ms is the measured time between frame valids
    // #15350000000; // ps timescale
    #6035; // ps timescale / 10000 for scaling down


    // Assert frame valid
    //  Image data now streaming (Not necessarily valid)
    camera_1_FV = 1'b1;

    // 50 us is the measured time between frame valid high and first line valid
    // #50000000; // ps timescale
    #500; // ps timescale / 100 for scaling down




    // Repeat to simulate 10 rows
    repeat(5) begin

      @(posedge pixel_clock)

      // Assert line valid
      //  Valid image pixels are now streaming
      camera_1_LV = 1'b1;

      // Pixel clock
      //  Allowing time for desire num pixels
      // repeat(2592) begin
      // repeat(1296) begin
      repeat(40) begin
        @(posedge pixel_clock);
      end

      // De-assert line valid
      //  Image pixels are still streaming but we are simulating horizontal blanking bits
      camera_1_LV = 1'b0;

      // 149.99 - 150.00 us is the measures time between line valids
      // #149950000; // ps timescale
      #1499; // ps timescale / 100 for scaling down


    end

    // 2480ns is the measures time between last line valid and frame valid going low
    #2480; // ps timescale

    // De-ssert frame valid
    //  Image data no longer streaming
    camera_1_FV = 1'b0;

  end

end

assign camera_1_pixel_clock = pixel_clock;


// Simulate pixels streaming from camera 0
always @(posedge pixel_clock) begin
  camera_1_pixel_in = camera_1_pixel_in + 12'b000000010000;
end








endmodule
