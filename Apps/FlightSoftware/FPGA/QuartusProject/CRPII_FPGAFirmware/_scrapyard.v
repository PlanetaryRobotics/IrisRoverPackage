module OneHertzClock
(
    input clkin,
    output reg clkout;
);
    reg [24:0] counter;

    initial begin
        counter = 0;
        clkout = 0;
    end
    always @(posedge clkin) begin
        if (counter == 0) begin
            counter <= 24999999; // We have a 50MHz base clock, need to edge twice per clock
            clkout <= ~clkout;
        end else begin
            counter <= counter -1;
        end
    end
endmodule

module InternalPixelClock
(
    input wire clk,
    input wire pixclk,
    input wire fv,
    input wire lv
    output reg int_pixclk
);
    // Do some pixel clock maths:
    // Raws values (from Table 10, Table 11):
    parameter COL_SIZE = 2591;
    parameter ROW_SIZE = 1943;
    parameter SW_LOWER = 1943; // default to SW_LOWER_MAX from 10. Is settable reg.
    parameter ROW_BIN = 0;
    parameter ROW_SKIP = 0;
    parameter COL_BIN = 0;
    parameter COL_SKIP = 0;
    // Settable registers (default to zero):
    parameter SW_UPPER = 0; // back-calculated from SW. Is settable register.
    parameter HB_reg = 0; // [PIXCLK] register setting
    parameter VB_reg = 0; // [rows] register setting
    // Computed Parameters:
    parameter IMAGE_WIDTH = 2 * $ceil( (COL_SIZE+1) / (2*(COL_SKIP+1)) );
    parameter IMAGE_HEIGHT = 2 * $ceil( (ROW_SIZE+1) / (2*(ROW_SKIP+1)) );
    parameter WDC = $ceil(80 / (1+COL_BIN));
    parameter HDR = 8; // always for all Row bins
    parameter HBmin = 346 * (ROW_BIN + 1) + 64 + (WDC/2); // [PIXCLK]
    parameter HB = HBmin > HB_reg ? HBmin : HB_reg; // [PIXCLK]
    parameter SW_calc = 2*16*SW_UPPER + SW_LOWER;
    parameter SW = SW_calc > 1 ? SW_calc : 1;
    parameter VBmin_calc = ((1 + SW) - IMAGE_HEIGHT); // [rows]
    parameter VBmin = VBmin_calc > 9 ? VBmin_calc : 9; // [rows]
    parameter VB = VBmin > VB_reg ? VBmin : VB_reg; // [rows]
    parameter tROW_calc = 2*(IMAGE_WIDTH/2 + HB); // [PIXCLK]
    parameter tROW_min = 2*(41 + 346*(ROW_BIN+1) + 99); // [PIXCLK]
    parameter tROW = tROW_calc > tROW_min ? tROW_calc : tROW_min; // [PIXCLK]
    parameter tFRAME = (IMAGE_HEIGHT + VB) * tROW; // [PIXCLK]
    parameter PIXCLKS_IN_FRAME = tFRAME; // how many pixel clocks in a frame

    // FPGA Clock is 50 MHz
    // PIXCLK we're emulating is 6MHz
    // 50/6 is 8.333
    // Which means we need to flip the signal (have an edge) at 12MHz
    // 50/12 = 4.1667 = 4 + 1/6 = (4+4+4+4+4+5)/6
    // so, we need to count to 4 5 times then count to 5, to average 4.1667
    reg [2:0] inner_counter = 3'b0; // counts to 3 or 4
    reg [2:0] outer_counter = 3'b0; // counts to 5 (0,1,2,3,4,5)
    reg [2:0] inner_counter_target = 3'd4;
    // Timer since the last time we realigned the clock with an FV pulse. Won't
    // do this more than 1 once per tFRAME:
    reg [23:0] realignment_timer = 24'b0;  // At default settings (max res.) this is 6.8M, so we'll use 24b
    reg realign = 1'b0;
    initial begin
        int_pixclk <= 0;
    end
    always @(posedge clk or realign) begin
        if(inner_counter < inner_counter_target) begin
            inner_counter <= inner_counter + 4'b1;
        end else begin // we're at the target:
            // Flip the signal:
            int_pixclk <= ~int_pixclk;
            // Advance the counters:
            inner_counter <= 3'd0;
            if(outer_counter < 3'd5) begin
                outer_counter <= outer_counter + 3'd1;
                inner_counter_target <= 3'd3;
            end else begin
                outer_counter <= 3'd0;
                inner_counter_target <= 3'd4;
            end

            // Advance the realignment timer (but stop counting above it b/c we
            // don't want to overflow).
            if(realignment_timer <= PIXCLKS_IN_FRAME){
                realignment_timer <= realignment_timer + 24'b1;
            }
        end

        // ! TODO: How do we realign?
        // Also need to reset the realignment flag.
    end

    // Realign the timer whenever fv rises IFF we've already done one image's
    // worth of cycles since the last one
    always @(posedge fv) begin
        // Only realign on an FV pulse if the last time we did it was > 1 frame
        // ago:
        if(realignment_timer > PIXCLKS_IN_FRAME) begin
            realign <= 1'b1;
        end
    end
    //! reset count to 0 at posedge FV if count is < frame time
endmodule

// N-bit Counter that increments on `posedge clk`, resets on reset
module ResettableCounter #(parameter N=8)
(
    input wire clk,
    input wire reset,
    output wire [N-1:0] count
);
    reg [N-1:0] count_reg = 0; // Internal register to hold the count
    assign count = count_reg; // Assign the internal register to the output

    always @(posedge clk or posedge reset) begin
        if(reset) begin
            count_reg <= 0;
        end else begin
            count_reg <= count_reg + 1;
        end
    end
endmodule

// Variant of `ResettableCounter` that resets on the positive edge of
// `posreset` but ONLY the positive edge (if `reset` is high, the count will
// still proceed).
module ResettableCounterPos #(parameter N=8)
(
    input wire clk,
    input wire posreset,
    output wire [N-1:0] count
);
    reg [N-1:0] count_reg = 0; // Internal register to hold the count
    assign count = count_reg; // Assign the internal register to the output

    reg reset_flag = 1'b0;
    reg reset_ack = 1'b0;
    always @(posedge posreset or posedge reset_ack) begin
        if(reset_ack) begin
            reset_flag <= 1'b0;
        end else begin
            reset_flag = 1'b1;
        end
    end

    always @(posedge clk or posedge reset_flag) begin
        if(reset_flag) begin
            count_reg <= 0;
            reset_ack <= 1'b1;
        end else begin
            count_reg <= count_reg + 1;
            reset_ack <= 1'b0;
        end
    end
endmodule