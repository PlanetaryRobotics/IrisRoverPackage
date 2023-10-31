// * I2C Controller FSM:
// FSM to control the I2C Core (via Wishbone, a.k.a. WB) and command it to
// execute a send or read operation.
// This requires writing and reading several registers *within* the I2C Core
// itself.
// This control logic is based on the (non-synthesizable) testbench for
// OpenCore's I2C Core:
// https://opencores.org/ocsvn/i2c/i2c/trunk/bench/verilog/tst_bench_top.v

module I2cController
# (
    // I2C Prescaler sets I2C Clock Freq:
    // Per 3.2 of the I2C Core docs, set this to int(f_clk/(4*f_i2c_sck_target))
    // Per 1. of the I2C Core docs, it only supports 100kbps and 400kbps modes (directly)
    // 50MHz / (4*100kHz) = 125

    parameter I2C_SCK_PRESCALER = 'd125,
    // Cycles to wait before executing each operation (based on delays in testbench):
    parameter PRE_OP_WAIT = 1
)   
(
    input clk,
    input rst,

    // Input Registers:
    // Data to send in the Send Operation:
    input [7:0] tx_data,
    // Whether this is the first operation in a transaction and we should
    // include a start bit:
    input i2c_start,
    // Whether this is the final operation in a transaction and we should
    // include a stop bit:
    input i2c_stop,
    // Whether to include a NACK in a read operation (final read):
    input i2c_read_nack,

    // Output Data:
    // Data read during a read operation:
    output [7:0] rx_data_out,

    // Control Flow Signals:
    // Pulse to tell us to begin a send operation. Must be low again before the send finishes.
    input tx_start,
    // Pulse to tell us to begin a read operation. Must be low again before the read finishes.
    input rx_start,
    // Flag to tell the outside that we just finished an operation. 1 clk long.
    output i2c_operation_done_out,
    // Flag to tell the outside that we're ready for business:
    output i2c_operation_ready_out
);

localparam I2C_SCK_PRESCALER_HI = I2C_SCK_PRESCALER & 'h00FF00;
localparam I2C_SCK_PRESCALER_LO = I2C_SCK_PRESCALER & 'h0000FF;

// * Output Registers:
reg i2c_operation_done = 1'b0;
assign i2c_operation_done_out = i2c_operation_done;
reg i2c_operation_ready = 1'b0;
assign i2c_operation_ready_out = i2c_operation_ready;
reg [7:0] rx_data = 8'b0;
assign rx_data_out = rx_data;

// * Addresses of Registers within the I2C Core:
// Addresses taken from testbench and `i2c_master_top.v`
// (docs pdf is from 2001 and for an older version)
localparam I2C_CORE_REG__PRER_LO = 3'b000; // Prescaler LSB
           I2C_CORE_REG__PRER_HI = 3'b001; // Prescaler MSB
           I2C_CORE_REG__CTR     = 3'b010; // Control Register
           I2C_CORE_REG__RXR     = 3'b011; // Read Data Register  (on reads)
           I2C_CORE_REG__TXR     = 3'b011; // Write Data Register (on writes)
           I2C_CORE_REG__CR      = 3'b100; // Command Register    (on writes)
           I2C_CORE_REG__SR      = 3'b100; // Status Register     (on reads)

// * I2C Core Command Register Bit Masks:
CR_MASK__START  = 8'b1_0_0_0_0_00_0;  // Generate Start Condition
CR_MASK__STOP   = 8'b0_1_0_0_0_00_0;  // Generate Stop Condition
CR_MASK__READ   = 8'b0_0_1_0_0_00_0;  // Read from I2C Device
CR_MASK__WRITE  = 8'b0_0_0_1_0_00_0;  // Write to I2C Device
CR_MASK__ACK    = 8'b0_0_0_0_1_00_0;  // ACK when a receiver, sent ACK (ACK=0) or NACK (ACK=1)
CR_MASK__IACK   = 8'b0_0_0_0_0_00_1;  // IACK, interrupt acknowledge. When set, clears pending interrupt.

// * I2C Core Status Register Bit Indices:
// (used to read return data only after we've read SR):
wire sr_bit__rxack  = wb_ctrl_rd_data[7];  // Received ACK from I2C Device (1= no ACK received, 0 = ACK received)
wire sr_bit__busy   = wb_ctrl_rd_data[6];  // I2C Bus is busy (1 after START detected, 0 after STOP detected)
wire sr_bit__tip    = wb_ctrl_rd_data[1];  // Transfer in Progress (1 while transferring data, 0 when transfer complete)
wire sr_bit__if     = wb_ctrl_rd_data[0];  // Interrupt Flag (interrupt is pending)

// * State Machine States:
localparam  BOOT                     = 5'd0,
            INIT_RESET               = 5'd1,
            // Program I2C Core Settings:
            WB_WRITE_PRER_LO         = 5'd2,
            WB_WRITE_PRER_LO_WAIT    = 5'd3,
            WB_WRITE_PRER_HI         = 5'd4,
            WB_WRITE_PRER_HI_WAIT    = 5'd5,
            WB_CHECK_PRER_LO_TRIG    = 5'd6,
            WB_CHECK_PRER_LO         = 5'd7,
            WB_CHECK_PRER_HI_TRIG    = 5'd8,
            WB_CHECK_PRER_HI         = 5'd9,
            WB_WRITE_CTR             = 5'd10,
            WB_WRITE_CTR_WAIT        = 5'd11,
            // Actual Control Loop: 
            IDLE                     = 5'd12,
            // Send Operation:
            WB_WRITE_TX_REG               = 5'd13,
            WB_WRITE_TX_REG_WAIT          = 5'd14,
            WB_WRITE_CMD_REG_TX           = 5'd15,
            WB_WRITE_CMD_REG_TX_WAIT      = 5'd16,
            WB_READ_STAT_REG_FOR_TX_DONE  = 5'd17,
            WB_CHECK_STAT_REG_FOR_TX_DONE = 5'd18,
            // Read Operation:
            WB_WRITE_CMD_REG_RX           = 5'd20,
            WB_WRITE_CMD_REG_RX_WAIT      = 5'd21,
            WB_READ_STAT_REG_FOR_RX_DONE  = 5'd22,
            WB_CHECK_STAT_REG_FOR_RX_DONE = 5'd23,
            WB_READ_RX_REG                = 5'd24,
            WB_READ_RX_REG_WAIT           = 5'd25;
            
reg [4:0] curr_state = BOOT;

// * Connection to Lower Level FSM for Controlling the I2C Core via Wishbone:
// Wishbone Controller Control Signals:
reg  [2:0] wb_ctrl_reg = 3'b0;                // I2C Core Register we're interacting with:
reg wb_ctrl_trig_wr = 1'b0, wb_ctrl_trig_rd = 1'b0; // Pulses to trigger WB to write/read to/from the given register
reg  [7:0] wb_ctrl_wr_data = 8'b0;            // Data to write to the register
wire [7:0] wb_ctrl_rd_data;                  // Data read from the register
wire wb_ctrl_done;                           // Pulse indicating Wishbone Controller finished doing whatever it was asked to do (1 clock long)
// Wishbone Interface Signals:
wire wb_stb, wb_we, wb_cyc, wb_ack, wb_inta;
wire [31:0] wb_adr;
wire [ 7:0] wb_dat_i, wb_dat_o;
WishboneController #(.dwidth(8), .awidth(32)) wishbone_i2c_controller(
    .clk(clk),
    .rst(rst),

    // Wishbone Controller Control Signals:
    .wb_ctrl_reg(wb_ctrl_reg),
    .wb_ctrl_trig_wr(wb_ctrl_trig_wr),
    .wb_ctrl_wr_data(wb_ctrl_wr_data),
    .wb_ctrl_trig_rd(wb_ctrl_trig_rd),
    .wb_ctrl_rd_data(wb_ctrl_rd_data),
    .wb_ctrl_done_out(wb_ctrl_done),

    // Wishbone Interface:
    .adr(wb_adr),
    .din(wb_dat_i),
    .dout(wb_dat_o),
    .cyc(wb_cyc),
    .stb(wb_stb),
    .we(wb_we),
    .sel(), // unused
    .ack(wb_ack),
    .err(1'b0), // unused
    .rty(1'b0) // unused
);

// * Connection between Wishbone Controller and I2C Core:
reg i2c_rstn = 1'b1; // Reset I2C Core (active low)
i2c_master_top i2c_top (
    // Wishbone Interface:
    .wb_clk_i(clk),
    .wb_rst_i(1'b0), // using arst instead
    .arst_i(i2c_rstn),
    .wb_adr_i(wb_adr[2:0]),
    .wb_dat_i(wb_dat_o),
    .wb_dat_o(wb_dat_i),
    .wb_we_i(wb_we),
    .wb_stb_i(wb_stb),
    .wb_cyc_i(wb_cyc),
    .wb_ack_o(wb_ack),
    .wb_inta_o(wb_inta),

    // I2C Signals:
    .scl_pad_i(scl),
    .scl_pad_o(scl_o),
    .scl_padoen_o(scl_oen),
    .sda_pad_i(sda),
    .sda_pad_o(sda_o),
    .sda_padoen_o(sda_oen)
);


// * Task blocks for triggering a wishbone read and write:
// Tells Wishbone to write to the given register *inside* the I2C Core:
task wb_trigger_write(input [2:0] i2c_core_reg, input [7:0] data);
begin
    wb_ctrl_reg <= i2c_core_reg;
    wb_ctrl_wr_data <= data;
    wb_ctrl_trig_wr <= 1'b1;  // Make sure to drive this low asap (before write is done)
end
endtask

// Tells Wishbone to read from the given register *inside* the I2C Core:
task wb_trigger_read(input [2:0] i2c_core_reg);
begin
    wb_ctrl_reg <= i2c_core_reg;
    wb_ctrl_trig_rd <= 1'b1; // Make sure to drive this low asap (before read is done)
end
endtask

// * State Machine Logic:
reg [36:0] state_timer = 37'b0; // 37b counter of how long we've been in the current state, capable of counting up to 30 minutes (9e10 cycles at 50MHz), way longer than any state should take.

task change_state(input [2:0] new_state);
begin
    state_timer <= 37'b0;    // reset timer for the next cycle
    curr_state <= new_state; // change the state for the next cycle
end
endtask

always @(posedge clk or posedge rst) begin
    if (rst) begin
        // Reset state:
        state_timer <= 37'b0;
        curr_state <= BOOT;

        i2c_operation_done <= 1'b0;
        i2c_operation_ready <= 1'b0;
        wb_ctrl_reg <= 3'b0;
        wb_ctrl_trig_wr <= 1'b0;
        wb_ctrl_trig_rd <= 1'b0;
        wb_ctrl_wr_data <= 8'b0;
    end else begin
        // Increment the state timer on the next cycle (only happens if state
        // timer isn't overridden later by a `change_state` call).
        state_timer <= state_timer + 37'b1;

        case(curr_state)

            // * I2C Core Configuration:
            BOOT: begin
                i2c_operation_ready <= 1'b0;
                i2c_rstn <= 1'b1; // negate reset
                if (state_timer > 1) begin
                    change_state(INIT_RESET);
                end
            end

            INIT_RESET: begin
                if(state_timer > 0) begin
                    i2c_rstn <= 1'b1; // negate reset
                    change_state(WB_WRITE_PRER_LO);
                end else begin
                    i2c_rstn <= 1'b0; // apply reset
                end
            end

            WB_WRITE_PRER_LO: begin
                // Write low byte of prescaler:
                if (state_timer > (PRE_OP_WAIT-1)) begin
                    wb_trigger_write(I2C_CORE_REG__PRER_LO, I2C_SCK_PRESCALER_LO);
                    change_state(WB_WRITE_PRER_LO_WAIT);
                end
            end

            WB_WRITE_PRER_LO_WAIT: begin
                // Wait for it to go through:
                wb_ctrl_trig_wr <= 1'b0;
                if (wb_ctrl_done) begin
                    change_state(WB_WRITE_PRER_HI);
                end
            end

            WB_WRITE_PRER_HI: begin
                // Write high byte of prescaler:
                if(state_timer > (PRE_OP_WAIT-1)) begin
                    wb_trigger_write(I2C_CORE_REG__PRER_HI, I2C_SCK_PRESCALER_HI);
                    change_state(WB_WRITE_PRER_HI_WAIT);
                end
            end

            WB_WRITE_PRER_HI_WAIT: begin
                // Wait for it to go through:
                wb_ctrl_trig_wr <= 1'b0;
                if (wb_ctrl_done) begin
                    change_state(WB_CHECK_PRER_LO_TRIG);
                end
            end

            WB_CHECK_PRER_LO_TRIG: begin
                // Trigger a read of the low byte of the prescaler to check it:
                if(state_timer > (PRE_OP_WAIT-1)) begin
                    wb_trigger_read(I2C_CORE_REG__PRER_LO);
                    change_state(WB_CHECK_PRER_LO);
                end
            end

            WB_CHECK_PRER_LO: begin
                // Check the low byte once it's been read:
                wb_ctrl_trig_rd <= 1'b0;
                if (wb_ctrl_done) begin
                    if (wb_ctrl_rd_data == WB_CHECK_PRER_LO) begin
                        // Value is correct, move on:
                        change_state(WB_CHECK_PRER_HI_TRIG);
                    end else begin
                        // Write didn't work, we need to do it all over again:
                        change_state(WB_WRITE_PRER_LO);
                    end
                end
            end

            WB_CHECK_PRER_HI_TRIG: begin
                // Trigger a read of the high byte of the prescaler to check it:
                if(state_timer > (PRE_OP_WAIT-1)) begin
                    wb_trigger_read(I2C_CORE_REG__PRER_HI);
                    change_state(WB_CHECK_PRER_HI);
                end
            end

            WB_CHECK_PRER_HI: begin
                // Check the low byte once it's been read:
                wb_ctrl_trig_rd <= 1'b0;
                if (wb_ctrl_done) begin
                    if (wb_ctrl_rd_data == WB_CHECK_PRER_HI) begin
                        // Value is correct, move on:
                        change_state(WB_WRITE_CTR);
                    end else begin
                        // Write didn't work, we need to do it all over again:
                        change_state(WB_WRITE_PRER_LO);
                    end
                end
            end

            WB_WRITE_CTR: begin
                // Write low byte of prescaler:
                if (state_timer > (PRE_OP_WAIT-1)) begin
                    wb_trigger_write(
                        I2C_CORE_REG__CTR,
                        8'b1_0_000000; // ENABLE yes, IEN no.
                    );
                    change_state(WB_WRITE_CTR_WAIT);
                end
            end

            WB_WRITE_CTR_WAIT: begin
                // Wait for it to go through:
                wb_ctrl_trig_wr <= 1'b0;
                if (wb_ctrl_done) begin
                    // Finally done with setup:
                    change_state(IDLE);
                end
            end


            IDLE: begin
                i2c_operation_ready <= 1'b1;
                i2c_operation_done <= 1'b0;  // deassert flag from prev. state
                if (tx_start) begin
                    change_state(WB_WRITE_TX_REG);
                end else if (rx_start) begin
                    change_state(WB_WRITE_CMD_REG_RX);
                end
            end


            //* Send Operation:
            WB_WRITE_TX_REG: begin
                // Give the I2C Core the data we want to send:
                if (state_timer > (PRE_OP_WAIT-1)) begin
                    wb_trigger_write(I2C_CORE_REG__TXR, tx_data);
                    change_state(WB_WRITE_TX_REG_WAIT);
                end
            end

            WB_WRITE_TX_REG_WAIT: begin
                // Wait for it to go through:
                wb_ctrl_trig_wr <= 1'b0;
                if (wb_ctrl_done) begin
                    change_state(WB_WRITE_CMD_REG_TX);
                end
            end

            WB_WRITE_CMD_REG_TX: begin
                // Tell I2C Core we want to send that data now:
                if (state_timer > (PRE_OP_WAIT-1)) begin
                    if (i2c_start & i2c_stop) begin
                        // Only byte of interaction. Generate start and stop condition:
                        wb_trigger_write( I2C_CORE_REG__CR,
                            (CR_MASK__WRITE | CR_MASK__START | CR_MASK__STOP)
                        );
                    end else if (i2c_stop) begin
                        // First byte of interaction. Generate start condition:
                        wb_trigger_write( I2C_CORE_REG__CR, (CR_MASK__WRITE | CR_MASK__START) );
                    end else if (i2c_stop) begin
                        // Last byte of interaction. Generate stop condition:
                        wb_trigger_write( I2C_CORE_REG__CR, (CR_MASK__WRITE | CR_MASK__STOP) );
                    end else begin
                        // Intermediate byte. Just write it:
                        wb_trigger_write(I2C_CORE_REG__CR, CR_MASK__WRITE);
                    end
                    change_state(WB_WRITE_CMD_REG_TX_WAIT);
                end
            end

            WB_WRITE_CMD_REG_TX_WAIT: begin
                // Wait for it to go through:
                wb_ctrl_trig_wr <= 1'b0;
                if (wb_ctrl_done) begin
                    change_state(WB_READ_STAT_REG_FOR_TX_DONE);
                end
            end

            WB_READ_STAT_REG_FOR_TX_DONE: begin
                // Trigger a read of the Status Register so we can see if the
                // I2C Transfer is still going on:
                if (state_timer > (PRE_OP_WAIT-1)) begin
                    wb_trigger_read(I2C_CORE_REG__SR);
                    change_state(WB_CHECK_STAT_REG_FOR_TX_DONE);
                end
            end

            WB_CHECK_STAT_REG_FOR_TX_DONE: begin
                // Check the Status Register for Transfer In-Progress once
                // it's read:
                wb_ctrl_trig_rd <= 1'b0;
                if (wb_ctrl_done) begin
                    if (sr_bit__tip) begin
                        // Transfer still in progress. Check again:
                        change_state(WB_READ_STAT_REG_FOR_TX_DONE);
                    end else begin
                        // Transfer done:
                        i2c_operation_done <= 1'b1; // flag that it's done
                        change_state(IDLE);
                    end
                end
            end

            
            //* Read Operation:
            WB_WRITE_CMD_REG_RX: begin
                // Tell I2C Core we want to send that data now:
                if (state_timer > (PRE_OP_WAIT-1)) begin
                    if (i2c_read_nack && i2c_stop) begin
                        wb_trigger_write( I2C_CORE_REG__CR,
                            (CR_MASK__READ | CR_MASK__ACK | CR_MASK__STOP) // ACK is NACK in RX mode
                        );
                    end else if (i2c_read_nack) begin
                        wb_trigger_write( I2C_CORE_REG__CR, (CR_MASK__READ | CR_MASK__ACK) ); // ACK is NACK in RX mode
                    end else begin
                        wb_trigger_write(I2C_CORE_REG__CR, CR_MASK__READ);
                    end
                    change_state(WB_WRITE_CMD_REG_RX_WAIT);
                end
            end

            WB_WRITE_CMD_REG_RX_WAIT: begin
                // Wait for it to go through:
                wb_ctrl_trig_wr <= 1'b0;
                if (wb_ctrl_done) begin
                    change_state(WB_READ_STAT_REG_FOR_RX_DONE);
                end
            end

            WB_READ_STAT_REG_FOR_RX_DONE: begin
                // Trigger a read of the Status Register so we can see if the
                // I2C Transfer is still going on:
                if (state_timer > (PRE_OP_WAIT-1)) begin
                    wb_trigger_read(I2C_CORE_REG__SR);
                    change_state(WB_CHECK_STAT_REG_FOR_RX_DONE);
                end
            end

            WB_CHECK_STAT_REG_FOR_RX_DONE: begin
                // Check the Status Register for Transfer In-Progress once
                // it's read:
                wb_ctrl_trig_rd <= 1'b0;
                if (wb_ctrl_done) begin
                    if (sr_bit__tip) begin
                        // Transfer still in progress. Check again:
                        change_state(WB_READ_STAT_REG_FOR_RX_DONE);
                    end else begin
                        // Now we can move on to fetching the read data from the I2C Core:
                        change_state(WB_READ_RX_REG);
                    end
                end
            end

            WB_READ_RX_REG: begin
                // Trigger a read of the Status Register so we can see if the
                // I2C Transfer is still going on:
                if (state_timer > (PRE_OP_WAIT-1)) begin
                    wb_trigger_read(I2C_CORE_REG__RX);
                    change_state(WB_READ_RX_REG_WAIT);
                end
            end

            WB_READ_RX_REG_WAIT: begin
                // Check the Status Register for Transfer In-Progress once
                // it's read:
                wb_ctrl_trig_rd <= 1'b0;
                if (wb_ctrl_done) begin
                    // We got the data:
                    // Copy it to rx_data:
                    rx_data <= wb_ctrl_rd_data;
                    // Flag that we're done with the op:
                    i2c_operation_done <= 1'b1;
                    // And move on:
                    change_state(IDLE);
                end
            end


            default: begin
                change_state(IDLE);
            end

        endcase
    end
end

endmodule
