`timescale 10ns/100ps // timescale/precision
`include "image_sel_state.v"
module testbench;

imaging_mode_state_machine(
    input wire clk,
    input wire wms, // Imaging Mode Select from WD
    // Settings determined by the mode:
    output reg reduce_resolution, // whether or not to take reduced resolution images
    output reg[1:0] cam_sel_bitmask, // which cameras to use lsb is cam1, msb is cam2. 1 means use
    output reg[1:0] exposure_level // 0-3/3 exposure level
);

// TEST CLOCK:
reg clk;
initial begin
    clk = 0;
end
always begin
    #2 clk = ~clk;  // 50MHz = 20ns period = 2 10ns time-units
end

// TEST SIGNAL:
initial begin
    wms = 1'b0;
    
end

endmodule