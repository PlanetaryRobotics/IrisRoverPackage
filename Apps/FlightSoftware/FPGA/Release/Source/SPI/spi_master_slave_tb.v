

`timescale 1ns/10ps
module spi_master_slave_tb;
    reg reset;
    reg clk = 1'b0;
    reg mlb = 1'b0;
    reg start_SPI_transfer = 1'b0;
    reg multiple_byte_transfer = 1'b0;
    reg [7:0] master_transmission_data = 8'b00000000;
    reg [1:0] clk_to_sck_divisor = 0;
    wire MISO;
    wire SS;
    wire SCK;
    wire MOSI;
    wire Mdone;
    wire [7:0] Mrdata;
	reg ten = 1'b0;
    reg [7:0] slave_transmission_data = 8'b00000000;
    wire SLVdone;
    wire [7:0] SLVrdata;


// End simulation
initial #10000 $stop;


// Target System Clock
parameter PERIOD = 20;  // 50MHz clock - 20ns period
parameter real DUTY_CYCLE = 0.5;
initial begin
    repeat(1000)
    begin
        clk = 1'b0;
        #(PERIOD-(PERIOD*DUTY_CYCLE));
        clk = 1'b1;
        #(PERIOD*DUTY_CYCLE);
    end
end


//uut MASTER instantiation
    spi_master MAS (
        .reset(reset),
        .clk(clk),
        .mlb(mlb),
        .start_SPI_transfer(start_SPI_transfer),
        .multiple_byte_transfer(multiple_byte_transfer),
        .transmit_data(master_transmission_data),
        .clk_to_sck_divisor(clk_to_sck_divisor),
        .MISO(MISO),
        .SS(SS),
        .SCK(SCK),
        .MOSI(MOSI),
        .done(Mdone),
        .received_data(Mrdata));
//uut SLAVE instantiation
	spi_slave SLV (
        .reset(reset),
        .ten(ten),
        .tdata(slave_transmission_data),
        .mlb(mlb),
        .SS(SS),
        .SCK(SCK),
        .MOSI(MOSI),
        .MISO(MISO),
        .done(SLVdone),
        .rdata(SLVrdata));



// SPI Flash Interface Stimulus - 8 bit
initial begin

  // Reset
  #10 reset = 1'b0;
  #100 reset = 1'b1;


  // Configure SPI Core
  clk_to_sck_divisor = 2'b00;
  mlb = 1'b1; // MSB of data transferred first



  // Indicate to SPI master there will be only a single SPI cycle
  multiple_byte_transfer = 1'b0;

  // Load data to master transmission data register
  master_transmission_data = 8'b10101010;

  // Tell SPI master to start
  start_SPI_transfer = 1'b1;

  // Wait for byte to transfer
  @(posedge Mdone);

  // Tell SPI master we are done with SPI cycles
  start_SPI_transfer = 1'b0;

  // Assert
  // - Slave received data should match master's send
  if( SLVrdata != master_transmission_data ) begin
    $display("Master sent: %0b, Slave received: %0b", master_transmission_data, SLVrdata);
    // $stop;
  end



  #1000;


  // Indicate to SPI master there will be multiple SPI cycles
  multiple_byte_transfer = 1'b1;

  // Load data to master transmission data register
  master_transmission_data = 8'b11001100;

  // Tell SPI master to start
  start_SPI_transfer = 1'b1;

  // Wait for byte to transfer
  @(posedge Mdone);

  // // Assert
  // // - Slave received data should match master's send
  // if( SLVrdata != master_transmission_data ) begin
  //   $display("%0t - Master sent: %0b, Slave received: %0b", $time, master_transmission_data, SLVrdata);
  //   // $stop;
  // end

  // Indicate to SPI master this is the final SPI cycle
  multiple_byte_transfer = 1'b0;

  // Load data to master transmission data register
  master_transmission_data = 8'b11110000;

  // Wait for byte to transfer
  @(posedge Mdone);

  // Tell SPI master we are done with SPI cycles
  start_SPI_transfer = 1'b0;

  // Assert
  // - Slave received data should match master's send
  if( SLVrdata != master_transmission_data ) begin
    $display("Master sent: %0b, Slave received: %0b", master_transmission_data, SLVrdata);
    // $stop;
  end



  #1000;



  // Indicate to SPI master there will be multiple SPI cycles
  multiple_byte_transfer = 1'b1;

  // Load dummy data to master transmission data register
  master_transmission_data = 8'b00000000;

  // Load data to slave transmission data register
  slave_transmission_data = 8'b00001111;

  // Indicate to SPI slave to enable transmissions during the following cycles
  ten = 1'b1;

  // Tell SPI master to start
  start_SPI_transfer = 1'b1;

  // Wait for byte to transfer
  @(posedge Mdone);

  // Load data to slave transmission data register
  slave_transmission_data = 8'b00101011;

  // Wait for byte to transfer
  @(posedge Mdone);

  // Assert
  // - Master received data should match slave's send
  if( Mrdata != slave_transmission_data ) begin
    $display("Master received: %0b, Slave sent: %0b", Mrdata, slave_transmission_data);
    // $stop;
  end

  // Indicate to SPI master this is the final SPI cycle
  multiple_byte_transfer = 1'b0;

  // Load data to slave transmission data register
  slave_transmission_data = 8'b01010101;

  // Wait for byte to transfer
  @(posedge Mdone);

  // Assert
  // - Master received data should match slave's send
  if( Mrdata != slave_transmission_data ) begin
    $display("Master received: %0b, Slave sent: %0b", Mrdata, slave_transmission_data);
    // $stop;
  end

  // Indicate to SPI slave we are done transmitting
  ten = 1'b0;

  // Tell SPI master we are done with SPI cycles
  start_SPI_transfer = 1'b0;







  #10000;



  // //################################################
  // // Set Extended Address Enable to high, BAR[7] - Enables 4 byte addressing
  // //  Set Bank Address Register (BAR) Command
  // //    Bank Register Write (BRWR 17h)
  // //    Instruction = 8'h17;
  // //    Data = 8'b10000000;
  // //    No preceding Write Enable (WREN) command necessary
  //
  // // Indicate an SPI transfer of more than one byte
  // multiple_byte_transfer = 1'b1;
  //
  // // Load a byte to transmission data register
  // master_transmission_data = 8'h17;
  //
  // // Start sending
  // #100  start_SPI_transfer = 1'b1;
  // #100  start_SPI_transfer = 1'b0;
  //
  // // Wait for byte to transfer
  // @(posedge Mdone);
  //
  // // Pulled low to indicate last byte of transfer
  // multiple_byte_transfer = 1'b0;
  //
  // // Load byte to transmission data register
  // master_transmission_data = 8'b10000000;
  //
  // // Wait for byte to transfer
  // @(posedge Mdone);
  //
  // //################################################
  //
  //
  // #1000;
  //
  //
  // //################################################
  // // Verify Extended Address Enable to high, BAR[7]
  // //  Read Bank Address Register (BAR) Command
  // //    Bank Register Read (BRRD 16h)
  // //    Instruction = 8'h16;
  //
  // // Indicate an SPI transfer of more than one byte
  // multiple_byte_transfer = 1'b1;
  //
  // // Load a byte to transmission data register
  // master_transmission_data = 8'h16;
  //
  // // Start transfer
  // #100  start_SPI_transfer = 1'b1;
  // #100  start_SPI_transfer = 1'b0;
  //
  // // Wait for byte to transfer
  // @(posedge Mdone);
  //
  //
  // // Pull transmission buffer to default state
  // master_transmission_data = 8'hFF;
  //
  // // Pulled low to indicate last byte to transfer
  // multiple_byte_transfer = 1'b0;
  //
  // // FOR VERILOG SPI SLAVE CORE ONLY
  // ten = 1'b1;
  // s_tdata = 8'b01011010; // Example data that would be sent from slave
  //
  // // Wait for single Bank Address Register byte to transfer from slave
  // @(posedge Mdone);
  //
  //
  // // Verify master has received byte
  // // assert (Mrdata == s_tdata);
  //
  // //################################################
  //
  //
  // #1000;


end


endmodule
