/*
Determines what imaging mode we should be in based on pulse commands from the
WD:

WD only has 1 wire to command FPGA.
Don't want to change interrupt behavior of WD this late in the game by adding a
full 1-Wire comms suite. Instead we have a simple protocol:

- Watchdog commands what "imaging mode" (state) the FPGA should be in.
- Watchdog sends low-high-low pulses that indicate what mode to be in.
    - Number of consecutive pulses is the mode.
    - Pulse length is arbitrary. It just needs to be > a couple 50MHz clock
    cycles but less than PULSE_COUNTER_RESET_TIME (1s).
- If the signal line is low for > PULSE_COUNTER_RESET_TIME (def. 1s),
the next time a pulse is received the count starts from 0.
    - NOTE: We *don't* reset the mode before this (i.e. once the low time is
    >1s) b/c we want to persist the previous mode setting until the WD starts
    giving us a new setting (more pulses).
- The mode wraps around if pulsed too many times, so we're always in a valid mode.
*/

// TODO: Fix combo & seq logic usage
// TODO: Make sure the *output* settings don't change while it's being inc'd (i.e. only apply after a wait of X has occurred since the last pulse)
    // i.e. we just need a settings_valid flag that goes high X cycles after the last pulse
// TODO: Add metastability protection layer and 3-clk averaging for bounce protection (already synchronized - or should be)
module imaging_mode_state_machine
#(
    // Max time (clock pulses) between pulses before we start counting from 0
    // next time we get a pulse:
    parameter PULSE_COUNTER_RESET_TIME = 26'd50_000_000 // 1s at 50MHz clock
)
(
    input wire clk,
    input wire wms, // Imaging Mode Select from WD
    // Settings determined by the mode:
    output reg reduce_resolution, // whether or not to take reduced resolution images
    output reg[1:0] cam_sel_bitmask, // which cameras to use lsb is cam1, msb is cam2. 1 means use
    output reg[1:0] exposure_level // 0-3/3 exposure level
);
	// Declare the state register to be "safe" to implement
	// a safe state machine that can recover gracefully from
	// an illegal state (by returning to the reset state).
	(* syn_encoding = "safe" *) reg [3:0] mode;

	// IMAGING MODES:
	parameter FULL_RES__CAM_1__LOW_EXPOSURE = 4'd0,
              FULL_RES__CAM_1__MED_EXPOSURE = 4'd1,
              FULL_RES__CAM_1__HIGH_EXPOSURE = 4'd2,
              FULL_RES__CAM_2__LOW_EXPOSURE = 4'd3,
              FULL_RES__CAM_2__MED_EXPOSURE = 4'd4,
              FULL_RES__CAM_2__HIGH_EXPOSURE = 4'd5,
              LOW_RES__BOTH_CAM__LOW_EXPOSURE = 4'd6,
              LOW_RES__BOTH_CAM__MED_EXPOSURE = 4'd7,
              LOW_RES__BOTH_CAM__HIGH_EXPOSURE = 4'd8;
    parameter MAX_MODE = LOW_RES__BOTH_CAM__HIGH_EXPOSURE;

    // Semaphores:
    // Counter tells pulse listener that the next pulse count should start from
    // 0:
    reg reset_mode;
    // Pulse listener tells counter that it's processsed the reset and it can
    // deassert reset:
    reg reset_mode_ack;

    // LOW COUNTER:
    // Check for how long we've been low:
    reg [25:0] low_counter;
    initial begin
        low_counter = 0;
    end
    always @(posedge clk) begin
        if(reset_mode_ack) begin
            reset_mode <= 0;
        end
        if (wms) begin
            // If WMS is high, we reset the count:
            low_counter <= 0;
        end else if (low_counter < PULSE_COUNTER_RESET_TIME) begin
            low_counter <= low_counter + 1;
        end else begin
            // We've been low for >1s. Tell the Pulse listener that the next
            // time it gets a pulse, it should first reset the count to 0:
            reset_mode <= 1;
        end
    end

    // WD PULSE LISTENER:
    // Check for the start of a pulse from the WD:
    initial begin
        mode = 0;
    end
    always @(posedge wms) begin
        if(reset_mode) begin
            // The signal was low for >1s prior to this pulse, so we should
            // start counting at 0:
            mode <= 0;
            // Tell counter that we've processed the reset request and it can
            // deassert:
            reset_mode_ack <= 1;
        end else begin
            if(mode == MAX_MODE) begin
                // Cycle back around:
                mode <= 0;
            end else begin
                // Advance state:
                mode <= mode + 1;
            end

            reset_mode_ack <= 0;
        end
    end

    // OUTPUT SETTINGS:
	// Output Settings depend only on the mode:
	always @ (mode) begin
		case (mode)
            // FULL_RES CAM 1:
            FULL_RES__CAM_1__LOW_EXPOSURE: begin
                cam_sel_bitmask = 2'b01;
                reduce_resolution = 1'b0;
                exposure_level = 2'b01;
            end
            FULL_RES__CAM_1__MED_EXPOSURE: begin
                cam_sel_bitmask = 2'b01;
                reduce_resolution = 1'b0;
                exposure_level = 2'b10;
            end
            FULL_RES__CAM_1__HIGH_EXPOSURE: begin
                cam_sel_bitmask = 2'b01;
                reduce_resolution = 1'b0;
                exposure_level = 2'b11;
            end
            // FULL_RES CAM 2:
            FULL_RES__CAM_2__LOW_EXPOSURE: begin
                cam_sel_bitmask = 2'b10;
                reduce_resolution = 1'b0;
                exposure_level = 2'b01;
            end
            FULL_RES__CAM_2__MED_EXPOSURE: begin
                cam_sel_bitmask = 2'b10;
                reduce_resolution = 1'b0;
                exposure_level = 2'b10;
            end
            FULL_RES__CAM_2__HIGH_EXPOSURE: begin
                cam_sel_bitmask = 2'b10;
                reduce_resolution = 1'b0;
                exposure_level = 2'b11;
            end
            // LOW_RES BOTH CAMERAS:
            LOW_RES__BOTH_CAM__LOW_EXPOSURE: begin
                cam_sel_bitmask = 2'b11;
                reduce_resolution = 1'b1;
                exposure_level = 2'b01;
            end
            LOW_RES__BOTH_CAM__MED_EXPOSURE: begin
                cam_sel_bitmask = 2'b11;
                reduce_resolution = 1'b1;
                exposure_level = 2'b10;
            end
            LOW_RES__BOTH_CAM__HIGH_EXPOSURE: begin
                cam_sel_bitmask = 2'b11;
                reduce_resolution = 1'b1;
                exposure_level = 2'b11;
            end
			default: begin
                cam_sel_bitmask = 2'b01;
                reduce_resolution = 1'b0;
                exposure_level = 2'b10;
            end
		endcase
	end
endmodule