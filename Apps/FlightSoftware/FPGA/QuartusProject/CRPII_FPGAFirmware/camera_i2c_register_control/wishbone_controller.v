// FSM to handle controlling a Core using a Wishbone Interface by
// writing to / reading from its configuration registers.
// Does not impl. full Wishbone spec, just what we need to command the I2C core.
// Based control logic in the Wishbone Master for OpenCore's (non-synthesizable)
// I2C Core testbench:
// https://opencores.org/ocsvn/i2c/i2c/trunk/bench/verilog/wb_master_model.v

module WishboneController
#(
    parameter dwidth = 32,
    parameter awidth = 32,
    parameter PRE_OP_DELAY = 1 // Number of cycles to wait before performing an operation
)
(
    .clk(clk),
    .rst(rst),

    // Wishbone Controller Control Signals:
    // I2C Core Register we're interacting with:
    input [2:0] wb_ctrl_reg,
    // Pulses to trigger WB to write/read to/from the given register
    // (pulse must be low again by the time the action is done)
    input wb_ctrl_trig_wr,
    input wb_ctrl_trig_rd,
    // Data to write to the register / data read from the register:
    input [7:0] wb_ctrl_wr_data,
    output [7:0] wb_ctrl_rd_data_out,
    // Pulse indicating Wishbone Controller finished doing whatever it was asked to do (1 clock long):
    output wb_ctrl_done_out,

    // Wishbone Interface:
    output [awidth   -1:0]	adr_out,  // Address
    input  [dwidth   -1:0]	din,  // Input Data (to write)
    output [dwidth   -1:0]	dout_out, // Output Data (was read)
    output cyc_out, // Valid Bus Cycle
    output stb_out, // Strobe Signal / Core Select
    output we_out,  // Write Enable
    output [dwidth/8-1:0] sel_out,  // Byte Select Signal
    input ack, // Bus Cycle Acknowledge
    input err,
    input rty
);

localparam sel_width = dwidth / 8;

// * States:
localparam  IDLE                   = 3'd0,
            START_WRITE_STAGING    = 3'd1,
            START_WRITE            = 3'd2,
            WRITE_WAIT             = 3'd3,
            START_READ_STAGING     = 3'd4,
            START_READ             = 3'd5,
            READ_WAIT              = 3'd6;
reg [2:0] curr_state = IDLE;

// * Output Registers:
reg                 wb_ctrl_done = 1'b0;
assign wb_ctrl_done_out = wb_ctrl_done;
reg                 cyc = 1'b0;
assign cyc_out = cyc;
reg                 stb = 1'b0;
assign stb_out = stb;
reg                 we = 1'b0;
assign we_out = we;
reg [sel_width-1:0] sel = {sel_width{1'b0}};
assign sel_out = sel;
reg [awidth-1:0]    adr = 1'b0;
assign adr_out = adr;
reg [dwidth-1:0]    dout = 1'b0;
assign dout_out = dout;
reg [       7:0]    wb_ctrl_rd_data = 8'b0;
assign wb_ctrl_rd_data_out = wb_ctrl_rd_data;

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
        curr_state <= IDLE;
        // Reset registers on resets:
        wb_ctrl_done <= 1'b0;
        cyc <= 1'b0;
        stb <= 1'b0;
        we <= 1'b0;
        sel <= {sel_width{1'b0}};
        adr <= {a_width{1'b0}};
        dout <= {d_width{1'b0}};
        wb_ctrl_rd_data <= 8'b0;
    end else begin
        // Increment the state timer on the next cycle (only happens if state
        // timer isn't overridden later by a `change_state` call).
        state_timer <= state_timer + 37'b1;

        // Perform state actions:
        case(curr_state)

            IDLE: begin
                wb_ctrl_done <= 1'b0;
                cyc <= 1'b0;
                stb <= 1'b0;
                we <= 1'b0;
                sel <= {sel_width{1'b0}};

                if (wb_ctrl_trig_wr) begin
                    change_state(START_WRITE_STAGING);
                end else if (wb_ctrl_trig_rd) begin
                    change_state(START_READ_STAGING);
                end
            end
            
            START_WRITE_STAGING: begin
                if (state_timer > (PRE_OP_DELAY-1)) begin
                    change_state(START_WRITE);
                end
            end

            START_WRITE: begin
                // Set all of the registers for a write:
                adr[2:0] <= wb_ctrl_reg;
                dout <= wb_ctrl_wr_data;
                cyc <= 1'b1;
                stb <= 1'b1;
                we <= 1'b1;
                sel <= {sel_width{1'b1}};
                // Then advance:
                change_state(WRITE_WAIT);
            end

            WRITE_WAIT: begin
                // Wait until we get an ACK:
                if(ack) begin
                    wb_ctrl_done <= 1'b1;
                    change_state(IDLE);
                end
            end

            START_READ_STAGING: begin
                if (state_timer > (PRE_OP_DELAY-1)) begin
                    change_state(START_READ);
                end
            end

            START_READ: begin
                // Set all of the registers for a write:
                adr[2:0] <= wb_ctrl_reg;
                dout <= {d_width{1'b0}};
                cyc <= 1'b1;
                stb <= 1'b1;
                we <= 1'b0;
                sel <= {sel_width{1'b1}};
                // Then advance:
                change_state(READ_WAIT);
            end

            READ_WAIT: begin
                // Wait until we get an ACK:
                if(ack) begin
                    // Grab the data:
                    wb_ctrl_rd_data <= din;
                    wb_ctrl_done <= 1'b1;
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
