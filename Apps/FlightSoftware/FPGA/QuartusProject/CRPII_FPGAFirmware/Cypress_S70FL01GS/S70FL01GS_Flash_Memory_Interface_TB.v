// SPI Flash Interface Testbench - Cypress S70FL01GS
//  - Dual-Died Cyprus S25FL512S
//  - Normal Data Rate (Single Data Rate - SDR) Operation
//  - Hardware Reset Active Low

// NOTE
// Simulation must be run with the altera_mf_ver library to simulate PLL
//  vsim -L altera_mf_ver work.S70FL01GS_Flash_Memory_Interface_TB


// TODO
//  This needs to be updates for the latest version of S70FL01GS_Flash_Memory_Interface




`timescale 1ns / 1ps

module S70FL01GS_Flash_Memory_Interface_TB();

  // System Inputs
  reg reset =1'b0;
  reg clk = 1'b0;

  // To Flash Memory (S70FL01GS) - SPI Slave
  wire flash_reset;
  wire flash_SCK;
  wire flash_CS1;
  wire flash_CS2;
  wire flash_SI;
  wire flash_SO;
  wire flash_WP;
  wire flash_HOLD;

  // wire flash_mem_interface_done;
  wire flash_mem_interface_spi_cycle_done;
  wire flash_mem_interface_output_valid;

  reg configure_flash = 1'b0;
  reg read_BAR = 1'b0;
  reg read_flash_ID = 1'b0;
  reg read_flash = 1'b0;
  reg write_enable = 1'b0;
  reg read_SR1 = 1'b0;
  reg write_flash = 1'b0;

  reg [7:0] input_data  = 8'd10;
  wire [7:0] output_data;

  reg [23:0] num_bytes = 24'h000000;

  reg [31:0] address;



// Keep testbench time
parameter PERIOD = 20; // ~50MHz clock - 20ns period
parameter real DUTY_CYCLE = 0.5;
initial begin  // Clock process for clk
    repeat(5000)
    begin
        clk = 1'b0;
        #(PERIOD-(PERIOD*DUTY_CYCLE)) clk = 1'b1;
        #(PERIOD*DUTY_CYCLE);
    end
end

// Be sure to terminate the simulation
initial #100000 $stop;


// UUT
S70FL01GS_Flash_Memory_Interface flash_mem (
  .clk(clk),
  .reset(reset),
  .flash_reset(flash_reset),
  .flash_SCK(flash_SCK),
  .flash_CS1(flash_CS1),
  .flash_CS2(flash_CS2),
  .flash_SI(flash_SI),
  .flash_SO(flash_SO),
  .flash_WP(flash_WP),
  .flash_HOLD(flash_HOLD),
  // .flash_mem_interface_done(flash_mem_interface_done),
  .flash_mem_interface_spi_cycle_done(flash_mem_interface_spi_cycle_done),
  .flash_mem_interface_output_valid(flash_mem_interface_output_valid),
  .configure_flash(configure_flash),
  .read_BAR(read_BAR),
  .read_flash_ID(read_flash_ID),
  .read_flash(read_flash),
  .write_enable(write_enable),
  .read_SR1(read_SR1),
  .write_flash(write_flash),
  .address(address),
  .input_data(input_data),
  .output_data(output_data),
  .num_bytes(num_bytes)
);



reg [7:0] spi_slave_tx_data = 8'h00; // Data to hercules
// wire hercules_spi_tx_data_valid; // Data sent is valid

wire [7:0] spi_slave_rx_data; // Data from hercules
// wire hercules_spi_rx_data_valid; // Data from hercules is valid


wire spi_data_valid; // SPI data is valid

reg spi_slave_tx_enable = 1'b0;

spi_slave SLV (
  .reset(reset),
  .ten(spi_slave_tx_enable),
  .tdata(spi_slave_tx_data),
  .mlb(1'b1), // Always left shift, always MSB first
  .SS(flash_CS1),
  .SCK(flash_SCK),
  .MOSI(flash_SI),
  .MISO(flash_SO),
  .done(spi_data_valid),
  .rdata(spi_slave_rx_data)
);


// Stimulus
initial begin
  #10 reset = 1'b0;
  #100 reset = 1'b1;



  #1000;

  configure_flash = 1'b1;

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's BRWR command
  if( spi_slave_rx_data != 8'h17 ) begin
    $display("Master sent: %0b, Slave received: %0b", 8'h17, spi_slave_rx_data);
    // $stop;
  end


  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's BAR setting
  if( spi_slave_rx_data != 8'b10000000 ) begin
    $display("Master sent: %0b, Slave received: %0b", 8'b10000000, spi_slave_rx_data);
    // $stop;
  end

  configure_flash = 1'b0;



  #1000;


  read_BAR = 1'b1;

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's BRRD command
  if( spi_slave_rx_data != 8'h16 ) begin
    $display("Master sent: %0b, Slave received: %0b", 8'h16, spi_slave_rx_data);
    // $stop;
  end

  // SPI slave - flash memory simulation dummy data for bank address register
  //  Data from flash will be shifted out after last byte of address
  spi_slave_tx_data = 8'b10000000; // BAR setting
  spi_slave_tx_enable = 1'b1;

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Master received data should match slave's send
  if( output_data != spi_slave_tx_data ) begin
    $display("Master received: %0b, Slave sent: %0b", output_data, spi_slave_tx_data);
    // $stop;
  end

  read_BAR = 1'b0;

  // @(posedge flash_mem_interface_spi_cycle_done);
  // // Assert
  // // - Master received data should match slave's send
  // if( output_data != spi_slave_tx_data ) begin
  //   $display("Master received: %0b, Slave sent: %0b", output_data, spi_slave_tx_data);
  //   // $stop;
  // end

  spi_slave_tx_enable = 1'b0;





  #1000;







  read_flash_ID = 1'b1;

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's REMS command
  if( spi_slave_rx_data != 8'h90 ) begin
    $display("Master sent: %0b, Slave received: %0b", 8'h90, spi_slave_rx_data);
    // $stop;
  end

  repeat(3) begin
    @(posedge flash_mem_interface_spi_cycle_done);
    // Assert
    // - Slave received data should match master's dummy address 24'h000000
    if( spi_slave_rx_data != 8'd0 ) begin
      $display("Master sent: %0b, Slave received: %0b", 8'd0, spi_slave_rx_data);
      // $stop;
    end
  end

  // SPI slave - flash memory simulation dummy data for ID
  //  Data from flash will be shifted out after last byte of address
  spi_slave_tx_data = 8'h19; // ID byte 1
  spi_slave_tx_enable = 1'b1;

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Master received data should match slave's send
  if( output_data != spi_slave_tx_data ) begin
    $display("Master received: %0b, Slave sent: %0b", output_data, spi_slave_tx_data);
    // $stop;
  end


  // NOTE
  //  THERE IS SOME BUG WHERE IF THIS IS NOT SET BEFORE THE LAST CYCLE
  //  AN EXTRA CYCLE IS GENERATED - THIS IS BAD
  read_flash_ID = 1'b0;


  // SPI slave - flash memory simulation dummy data for ID
  spi_slave_tx_data = 8'h35; // ID byte 2

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Master received data should match slave's send
  if( output_data != spi_slave_tx_data ) begin
    $display("Master received: %0b, Slave sent: %0b", output_data, spi_slave_tx_data);
    // $stop;
  end

  // SEE NOTE ABOVE
  // read_flash_ID = 1'b0;


  spi_slave_tx_enable = 1'b0;





  #1000;





  read_flash = 1'b1;
  address = 32'h10000001;
  num_bytes = 24'd10; // read 10 bytes

  // Wait for instruction + 4 address bytes
  // repeat(5) begin
  //   @(posedge flash_mem_interface_spi_cycle_done);
  // end


  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's send
  if( spi_slave_rx_data != 8'h13 ) begin
    $display("Master sent: %0b, Slave received: %0b", 8'h13, spi_slave_rx_data);
    // $stop;
  end

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's send
  if( spi_slave_rx_data != address[31:24] ) begin
    $display("Master sent: %0b, Slave received: %0b", address[31:24], spi_slave_rx_data);
    // $stop;
  end

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's send
  if( spi_slave_rx_data != address[23:16] ) begin
    $display("Master sent: %0b, Slave received: %0b", address[23:16], spi_slave_rx_data);
    // $stop;
  end

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's send
  if( spi_slave_rx_data != address[15:8] ) begin
    $display("Master sent: %0b, Slave received: %0b", address[15:8], spi_slave_rx_data);
    // $stop;
  end

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's send
  if( spi_slave_rx_data != address[7:0] ) begin
    $display("Master sent: %0b, Slave received: %0b", address[7:0], spi_slave_rx_data);
    // $stop;
  end



  // SPI slave - flash memory simulation dummy data
  //  Data from flash will be shifted out after last byte of address
  spi_slave_tx_data = 8'h00;
  spi_slave_tx_enable = 1'b1;


  // Read 10 bytes
  repeat(10) begin
    // SPI slave - flash memory simulation dummy data
    spi_slave_tx_data = spi_slave_tx_data + 1;

    @(posedge flash_mem_interface_spi_cycle_done);

    // Assert
    // - Master received data should match slave's send
    if( output_data != spi_slave_tx_data ) begin
      $display("Master received: %0b, Slave sent: %0b", output_data, spi_slave_tx_data);
      // $stop;
    end

  end


  read_flash = 1'b0;
  address = 32'h00000000;
  num_bytes = 24'd0; // read 10 bytes

  spi_slave_tx_enable = 1'b0;







  #1000;


  read_flash = 1'b1;
  address = 32'h01010101;
  num_bytes = 24'd20; // read 20 bytes

  // Wait for instruction + 4 address bytes
  repeat(5) begin
    @(posedge flash_mem_interface_spi_cycle_done);
  end

  // SPI slave - flash memory simulation dummy data
  spi_slave_tx_data = 8'h00;
  spi_slave_tx_enable = 1'b1;

  // Read 20 bytes from address
  repeat(20) begin
    // SPI slave - flash memory simulation dummy data
    spi_slave_tx_data = spi_slave_tx_data + 1;

    @(posedge flash_mem_interface_spi_cycle_done);

    // Assert
    // - Master received data should match slave's send
    if( output_data != spi_slave_tx_data ) begin
      $display("Master received: %0b, Slave sent: %0b", output_data, spi_slave_tx_data);
      // $stop;
    end

  end

  read_flash = 1'b0;
  address = 32'h00000000;
  num_bytes = 24'd0; // read 10 bytes

  spi_slave_tx_enable = 1'b0;



  // Good up to here


  #2000;




  // Enable flash to allow writes
  //  NOTE REQUIRED BEFORE EVERY WRITE FLASH
  write_enable = 1'b1;

  // Wait for instruction to send
  @(posedge flash_mem_interface_spi_cycle_done);

  // Assert
  // - Slave received data should match master's send
  if( spi_slave_rx_data != 8'h06 ) begin
    $display("Master sent: %0b, Slave received: %0b", 8'h06, spi_slave_rx_data);
    // $stop;
  end

  write_enable = 1'b0;




  #1000;


  read_SR1 = 1'b1;

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Slave received data should match master's RDSR1 command
  if( spi_slave_rx_data != 8'h05 ) begin
    $display("Master sent: %0b, Slave received: %0b", 8'h05, spi_slave_rx_data);
    // $stop;
  end

  // SPI slave - flash memory simulation dummy data for bank address register
  //  Data from flash will be shifted out after last byte of address
  spi_slave_tx_data = 8'b00000010; // Status register 1 setting with Write enable latch set high
  spi_slave_tx_enable = 1'b1;

  @(posedge flash_mem_interface_spi_cycle_done);
  // Assert
  // - Master received data should match slave's send
  if( output_data != spi_slave_tx_data ) begin
    $display("Master received: %0b, Slave sent: %0b", output_data, spi_slave_tx_data);
    // $stop;
  end

  read_SR1 = 1'b0;

  spi_slave_tx_enable = 1'b0;






  // Wait some time for flash to update configuration
  #500;


  write_flash = 1'b1;
  address = 32'h01020304;
  num_bytes = 24'd20;

  // Wait for instruction + 4 address bytes to send
  repeat(5) begin
    @(posedge flash_mem_interface_spi_cycle_done);
  end
  // $display ($time, flash_mem_interface_spi_cycle_done);


  // Write bytes to address
  repeat(20) begin
    // Dummy input data
    input_data = input_data + 1;

    @(posedge flash_mem_interface_spi_cycle_done);

    // Assert
    // - Slave received data should match master's send
    if( spi_slave_rx_data != input_data ) begin
      $display("Master sent: %0b, Slave received: %0b", input_data, spi_slave_rx_data);
      // $stop;
    end
  end

  write_flash = 1'b0;
  address = 32'h00000000;
  num_bytes = 24'd0; // read 10 bytes

  input_data = 8'd10;



  #2000;









  read_flash = 1'b1;
  address = 32'h01010101;
  num_bytes = 24'd20; // read 20 bytes

  // Wait for instruction + 4 address bytes
  repeat(5) begin
    @(posedge flash_mem_interface_spi_cycle_done);
  end


  // SPI slave - flash memory simulation dummy data
  spi_slave_tx_data = 8'h00;
  spi_slave_tx_enable = 1'b1;

  // @(posedge flash_mem_interface_ready_for_data);
  // @(posedge flash_mem_interface_output_valid);

  // Read 20 bytes from address
  repeat(20) begin
    // SPI slave - flash memory simulation dummy data
    spi_slave_tx_data = spi_slave_tx_data + 1;

    @(posedge flash_mem_interface_spi_cycle_done);

    // Assert
    // - Master received data should match slave's send
    if( output_data != spi_slave_tx_data ) begin
      $display("Master received: %0b, Slave sent: %0b", output_data, spi_slave_tx_data);
      // $stop;
    end
  end

  read_flash = 1'b0;
  address = 32'h00000000;
  num_bytes = 24'd0; // read 10 bytes

  spi_slave_tx_enable = 1'b0;




end


endmodule
