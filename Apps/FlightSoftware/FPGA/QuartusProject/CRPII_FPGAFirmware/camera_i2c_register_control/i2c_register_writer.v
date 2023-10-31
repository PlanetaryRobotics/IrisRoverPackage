// * I2C Register Writing FSM:
// FSM to control the high level transactions for writing an I2C register.
// NOTE: In I2C, writing so a register requires several individual send
// operations: Send Start+Address+WR, Send Register, Send Data+Stop.
// TODO: Optionally, checking that the I2C device actually stored the data (correctly) requires (per the manual and testbench):
// TODO: Send Start+Address+WR, Send Register, Send Start+Address+RD, Execute Read+NACK+Stop.

module I2cRegisterWriter(
    input clk,
    input rst,

    // Input Registers:
    input [6:0] i2c_addr,   // Address of the Target Device on the I2C Bus (only 7b)
    input [7:0] reg_addr,   // Address to write to in the I2C device
    input [7:0] reg_data,   // Data to write to the given register

    // Control Flow Signals:
    input reg_wr_start,     // Set to 1 to initiate a write. Make sure this is 0 before the write is done.
    output reg_wr_done,     // Flag indicating that the register write operation has completed. 1 clk long pulse.
    output reg_wr_ready     // Flag indicating that we're ready to perform register write operations
);


// * States:
localparam  IDLE                   = 4'd0,
            I2C_SEND_SADDR         = 4'd1,
            I2C_SEND_SADDR_WAIT    = 4'd2,
            I2C_SEND_REG_ADDR      = 4'd3,
            I2C_SEND_REG_ADDR_WAIT = 4'd4,
            I2C_SEND_REG_DATA      = 4'd5,
            I2C_SEND_REG_DATA_WAIT = 4'd6;
reg [3:0] curr_state = IDLE;


// * Connection to Lower Level FSM for Controlling each I2C Send operation:
// Data to send over the I2C Bus in Each Send Operation:
reg [7:0] tx_data = 8'd0;
// Whether this is the first transmission and a start bit should be included:
reg i2c_start = 1'b0;
// Whether this is the last transmission and a stop bit should be included:
reg i2c_stop = 1'b0;
// Whether to include a NACK in a read operation (final read):
reg i2c_read_nack = 1'b0;
// Pulse to indicate we want a send operation to start:
// (needs to be low again before the send operation is complete)
reg tx_start = 1'b0;
// Flag telling us a send operation has completed. 1 clk long:
wire i2c_send_done;
// Flag telling us the sender is set up and ready to go:
wire i2c_send_ready;
I2cSender i2c_sender(
    .clk(clk),
    .rst(rst),
    .tx_data(tx_data),
    .i2c_start(i2c_start),
    .i2c_stop(i2c_stop),
    .i2c_read_nack(i2c_read_nack),
    .tx_start(tx_start),
    .i2c_send_done(i2c_send_done),
    .i2c_send_ready(i2c_send_ready)
);


// * Internal Control Registers:
reg int_reg_wr_done = 1'b0;
assign reg_wr_done = int_reg_wr_done;
reg int_reg_wr_ready = 1'b0;
// Both our FSM and the I2C send FSM need to be ready for us to be ready to receive commands:
assign reg_wr_ready = int_reg_wr_ready & i2c_send_ready;


// * State Machine Logic:
always @(posedge clk or posedge rst) begin
    if (rst) begin
        // Reset registers on resets:
        tx_data <= 8'd0;
        i2c_start <= 1'b0;
        i2c_stop <= 1'b0;
        tx_start <= 1'b0;
        int_reg_wr_done <= 1'b0;
        int_reg_wr_ready <= 1'b0;
        curr_state <= IDLE;
    end else begin
        
        case(curr_state)

            IDLE: begin
                // Reset the done flag so it's only 1 pulse long:
                int_reg_wr_done <= 1'b0;
                // Let the outside know we're ready:
                int_reg_wr_ready <= 1'b1;
                if (reg_wr_start && i2c_send_ready) begin
                    curr_state <= I2C_SEND_SADDR;
                end
            end

            I2C_SEND_SADDR: begin
                // Tiny holding state to set up and trigger the SADDR Send:
                // I2C requires the LSB of a SADDR transfer to be 0 to indicate
                // a write and 1 to indicate a read:
                tx_data <= {i2c_addr, 1'b0};
                i2c_start <= 1'b1; // First transfer
                i2c_stop <= 1'b0;
                tx_start <= 1'b1; // Trigger the send operation.
                curr_state <= I2C_SEND_SADDR_WAIT;
            end

            I2C_SEND_SADDR_WAIT: begin
                // Wait for SADDR send to complete.
                tx_start <= 1'b0; // Deassert so it's off before the send ends
                if (i2c_send_done) begin
                    curr_state <= I2C_SEND_REG_ADDR;
                end
            end

            I2C_SEND_REG_ADDR: begin
                // Tiny holding state to set up and trigger the REG_ADDR Send:
                tx_data <= reg_addr;
                i2c_start <= 1'b0;
                i2c_stop <= 1'b0;
                tx_start <= 1'b1; // Trigger the send operation.
                curr_state <= I2C_SEND_REG_ADDR_WAIT;
            end

            I2C_SEND_REG_ADDR_WAIT: begin
                // Wait for REG_ADDR send to complete.
                tx_start <= 1'b0; // Deassert so it's off before the send ends
                if (i2c_send_done) begin
                    curr_state <= I2C_SEND_REG_DATA;
                end
            end

            I2C_SEND_REG_DATA: begin
                // Tiny holding state to set up and trigger the REG_DATA Send:
                tx_data <= reg_data;
                i2c_start <= 1'b0;
                i2c_stop <= 1'b1; // Final transfer of this operation.
                tx_start <= 1'b1; // Trigger the send operation.
                curr_state <= I2C_SEND_REG_DATA_WAIT;
            end

            I2C_SEND_REG_DATA_WAIT: begin
                // Wait for REG_DATA send to complete.
                tx_start <= 1'b0; // Deassert so it's off before the send ends
                if (i2c_send_done) begin
                    // We're done.
                    int_reg_wr_done <= 1'b1; // Flag that we're done
                    curr_state <= IDLE; // ... and return to IDLE
                end
            end

            default: begin
                curr_state <= IDLE;
            end

        endcase

    end
end

endmodule
