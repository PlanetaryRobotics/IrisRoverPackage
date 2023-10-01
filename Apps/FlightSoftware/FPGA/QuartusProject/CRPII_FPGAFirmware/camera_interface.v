// FSM and support logic for interfacing with the Camera Sensors

// Store settings per image (mode etc) - don't apply changes until capturing a new image.

// Designed with input synchronization and filtering, and line states to help 
// with immunity to signal noise and phase shifts on the incoming parallel bus.
// Also designed with some protections against SEU in mind.

// TODO: Check for FIFO full and don't advance internal state (which pixels and
// lines are done) until we've actually written the data. (check for almost_full too?)

module CameraSensorInterface
#(
    // * Config Variables:
    parameter integer SYNC_STAGES = 'd10, // Number of input synchronization stages    
    parameter integer PIX_SAMPLING_DELAY = 'd3, // Number of system clocks to wait after PIX_CLK falls before sampling the pixel data (if you want this to be 0, you need to remove the associated delay stage - setting this to 0 won't work).
    parameter integer VALID_LINE_SIZE = 12'd2592, // Number of valid pixels in a line of a frame
    parameter integer LINES_IN_FRAME = 12'd1944, // Number of lines in each full frame

    // Number of frames to interleave into each image (bigger = more relief for
    // Flash FSM & more frames in "video", smaller = less motion blur and
    // faster image capture time). Needs to be at least 1.
    // Max 32 (max for bits in `interleaved_frame_index`).
    // To perform correctly, it should be a factor of `LINES_IN_FRAME`.
    parameter integer N_INTERLEAVED_FRAMES = 12'd8;
)
(
    // Signals from camera sensor:
    input wire clk,
    input wire FV,
    input wire LV,
    input wire PIXCLK,
    input wire [11:0] PIXDATA,
    input wire FIFO_WR_REQ  // Signal used to enter data into the FIFO,

    // Context information:
    input wire [7:0] imaging_mode,

    // I/O control & status signals:
    input wire request_image,
    output wire image_started,
    output wire image_finished
    
    // Data to be saved into the FIFO (pixel data or header info)
    output wire [7:0] data_for_fifo; 
);

// Count clocks since last FIFO entry

// Count clocks since last FV

// Count clocks since last LV

// Signal for valid pixels:
assign PV = FV & LV;

// DIAGNOSTIC COUNTS FOR PRODUCING TIMING IMAGES:
reg [14:0] lv_clk_count; // Number of sys clocks since last LV posedge
reg [14:0] pv_clk_count; // Number of sys clocks since last PV 
reg [14:0] pv_clk_count; // Number of sys clocks since last PV 
reg [14:0] pixclk_clk_count; // Number of sys clocks since last PIXCLK negedge

reg [25:0] fv_clk_count; // Number of sys clocks since last FV posedge
ResettableCounter #(.N(26)) fv_counter(.clk(clk), .count(fv_clk_count), .reset_on_pos(FV));
wire fv_clk_count_reset

// Line data MUX:

always @(posedge clk or posedge FV) begin
    fv_clk_count = fv_clk_count + 1;
end




// *****************************
// * INPUT PROCESSING **********
// *****************************
// Continuous stream processing input signals.
// Not part of the command & control loop (purely inputs).


// * SIGNAL SYNCHRONIZATION AND CONDITIONING *
// Enabled by the fact that we get 4 full system clocks per PIXCLK and 8 between each rise.
integer i; // not real, just in the assembler
// Synchronizer data:
reg [14:0] sync [0:SYNC_STAGES]; // 12b data + 1 bit clk, 1 bit FV, 1 bit LV = 15b
wire [14:0] sync_out = sync[SYNC_STAGES];
// Data buffer:
reg [14:0] sync_buff [0:2];
// Indices in the sync busses (PIXDATA will always be 11:0):
localparam IDX_PIXCLK = 'd14;
localparam IDX_FV = 'd13;
localparam IDX_LV = 'd12;
always @(posedge clk) begin
    // Pass ALL incoming camera signals through a 10 stage flipflop synchronizer
    // chain (basically a shift register):
    //
    // This synchronizes everything to our system clock domain and prevents
    // metastability (more likely with our noisy signal over long cables).
    // 10 stage number is arbitrary but likely way more than needed
    // - no harm in picking a large number here since we don't care about the
    // propagation delay (so long as its uniform) but we DO care about ensuring
    // there are no stability issues in our signals.
    //
    // Concatenate all of our signals on a single bus so they're all handled together:
    // ** If changing this, MAKE SURE THE INDICES IDX_PIXCLK, IDX_FV, and IDX_LV match where you put data in this bus:
    sync[0] <= {PIXCLK, FV, LV, PIXDATA[11:0]};
    for(i = 1; i<=SYNC_STAGES; i++) begin
        sync[i] <= sync[i-1];
    end

    // Put all of the signals through a 3 stage buffer:
    // NOTE: we only have 4 sys clocks per pix clock edge, 8 per cycle.
    // Need a min of 3 buffer stages for "averaging" consensus. Don't want to
    // hit max sys clocks so 3 it is.
    sync_buff[0] <= sync_out;
    sync_buff[1] <= sync_buff[0];
    sync_buff[2] <= sync_buff[1];
end


// * Input "Averaging" (filter) *
// really just a 3bit moving average to get consensus for a signal.
// Done for all signals as a bus. Not necessarily used for all signals.
// Makes us immune to single-cycle blips.
wire [14:0] sync_avg_pre = (  // this signal is a pre-stage for edge-detection
      (sync_buff[2] & sync_buff[1])
    | (sync_buff[2] & sync_buff[0])
    | (sync_buff[1] & sync_buff[0])
);
reg [14:0] sync_avg;
always @(posedge clk) begin
    // Add 1 stage of delay to averaged signals to check for edges in the
    // averaged signals:
    // (we have `sync_avg` be the later of these two stages so the `sync_avg`
    // edge detectors in time with the `sync_avg` signal)
    sync_avg <= sync_avg_pre;
end


// * Edge Detectors *
// Detect edges on the synchronized bus outputs:
wire [14:0] sync_posedge = sync_buff[2] & ~sync_buff[1];
wire [14:0] sync_negedge = ~sync_buff[2] & sync_buff[1];
// Detect edges on the averaged bus outputs:
wire [14:0] sync_avg_posedge = sync_avg & ~sync_avg_pre;
wire [14:0] sync_avg_negedge = ~sync_avg & sync_avg_pre;

// * Key Synchronized Signals *
wire [11:0] sync_PIXDATA = sync_avg[11:0];
wire sync_LV = sync_avg[IDX_LV];
wire sync_FV = sync_avg[IDX_FV];

// * Important Transitions *
wire s_pixclk_fell = sync_avg_negedge[IDX_PIXCLK]; // a 1 sys-clock long pulse
wire s_line_start = sync_avg_posedge[IDX_LV]; // a 1 sys-clock long pulse
wire s_line_end = sync_avg_negedge[IDX_LV]; // a 1 sys-clock long pulse
wire s_frame_start = sync_avg_posedge[IDX_FV]; // a 1 sys-clock long pulse
wire s_frame_end = sync_avg_negedge[IDX_FV]; // a 1 sys-clock long pulse


// * PIXEL SAMPLING SIGNAL *
reg pix_sample_delay_pipe [0:PIX_SAMPLING_DELAY-1];
// Sample Pixel Pulse: a 1 sys-clock long pulse
// NOTE: this is just the FIFO write-clock, i.e. is now when we would sample a
// pixel, not *should* we sample this pixel - that's FIFO write-enable: `keep_pixel`.
wire sample_pixels_pulse = pix_sample_delay_pipe[PIX_SAMPLING_DELAY-1];
integer j;
always @(posedge clk) begin
    // Trigger a reading the pixels PIX_SAMPLING_DELAY system clocks after
    // pixel clock falls (give them time to finish transitioning -
    // accounts for any phase differences built up).
    // Data will stick around for about 8 clocks before transitioning so we can
    // safely make this a few wide.
    pix_sample_delay_pipe[0] <= s_pixclk_fell;
    for(j=0; j<PIX_SAMPLING_DELAY; j++) begin
        pix_sample_delay_pipe[j] <= pix_sample_delay_pipe[j-1];
    end
end


// * LINE LOGIC & PIXEL COUNTER *
// Track line state (treat it as a tiny FSM instead of just combinatorial logic):
reg line_active = 0; // does the pixel data represent data inside a line
wire line_complete = ~line_active;
// `pixels_elapsed_in_line`: how many pixels have could we *sampled* in this
// line since it started (ignoring whether we're actually sampling this line)
// Starts at 0 (*before* we've sampled any pixels).
// Counts up to `VALID_LINE_SIZE` (*after* we've sampled the last pixel in the line)
// Thus:
//  - before and during `sample_pixels_pulse`, this serves as the vector index
//    of the pixel being sampled.
//  - after `sample_pixels_pulse`, this serves as a count of the number of
//    pixels we've pushed into the FIFO.

reg [11:0] pixels_elapsed_in_line = 0;
always @(posedge clk) begin
    if (line_active) begin
        // Don't transition out of a line until the appropriate number of
        // pixels have elapsed (this helps ensure we always write a full line
        // of consecutive pixels):
        if(pixels_elapsed_in_line >= VALID_LINE_SIZE) begin  // >= not == to help recover from SEU
            line_active = 1'b0; // immediate
            pixels_elapsed_in_line <= 12'b0; // reset the count
        end else if(sample_pixels_pulse) begin
            // Set this using a "non-blocking" assignment so the value doesn't take
            // place until the next sys-clock (since we won't have actually sampled
            // it until then).
            pixels_elapsed_in_line <= pixels_elapsed_in_line + 12'b1;
        end
    end else if (sync_LV) begin
        // We're not in the line active state but the flag is now high.
        // Transition immediately:
        line_active = 1'b1;
    end
end


// * LINE COUNTER *
// NOTE: As a matter of uniform practice,
// ALL LINE COUNTS ARE ADVANCED DURING THE BLANKING INTERVAL FOLLOWING A
// LINE'S COMPLETION (more time to execute the logic and be sure about the inc)
reg [11:0] line_in_frame = 12'b0; // What line number are we in the frame (0-idx)
reg [11:0] prev_line_in_frame = 12'b0; // What was the last line in the frame (0-idx). Typ. l_i_f-1, except when we start and when we wrap around.
wire in_blanking_region = sync_PIXDATA[11:0] == 12'b0;
reg line_counter_can_be_incremented = 1'b0;
reg line_counter_incremented_at_end = 1'b0; // 1 sys-clock pulse letting observers know we just incremented the line_counter (at the end of a line)
reg frame_ended_pulse = 1'b0; // 1 sys-clock wide pulse telling observers the last line of the current frame is definitely done. 
always @(posedge clk) begin
    // NOTE: `line_complete` is involved in the following checks in order to 
    // prevent the line number from being incremented until we've read a full
    // line's worth of pixels into the FIFO (meaning we won't suddenly jump
    // lines)

    if(line_counter_incremented_at_end == 1'b1) begin
        // only let this persist for 1 sys-clock
        line_counter_incremented_at_end <= 1'b0;
    end

    // Increment the count on the line END during the blanking interval
    // because there are other signals we can cross check to make sure this
    // isn't errant. Also gives us more time to execute the increment logic
    // (the duration of the blanking interval).
    if( // Line is over when:
        !sync_LV // LV is low
        && line_complete // line is no longer active (we've read enough pixels)
        && in_blanking_region // all of the data bits are 0 (we're in blanking)
        && line_counter_can_be_incremented // make sure this only happens once
    ) begin
        line_counter_can_be_incremented = 1'b0;
    
        prev_line_in_frame = line_in_frame;
        line_in_frame = line_in_frame + 12'b1;

        line_counter_incremented_at_end <= 1'b1; // trigger this after increment
    end

    if(line_active) begin
        // Now that we're back in a line, allow the line counter to be
        // incremented again once we're out:
        line_counter_can_be_incremented = 1'b1;
    end

    if(frame_ended_pulse == 1'b1) begin
        frame_ended_pulse <= 1'b0;
    end

    // Only reset during the blanking region between frames:
    // (this is a long period rel. to sys clk and we don't need to act fast on
    // it so its okay to have a lot of combinatorial logic involved):
    if(
        !sync_FV // FV is low
        && !sync_LV // LV is low
        && in_blanking_region
        && line_complete // we've read all the data we need to read so it's okay to change this
    ) begin
        prev_line_in_frame = line_in_frame;
        line_in_frame = 12'b0;
        frame_ended_pulse <= 1'b1; // send after resetting counts
    end
end



// *****************************
// * FRAME PROCESSING **********
// *****************************
// Process frame data for the command & control loop
// (e.g. interleaving lines, adding headers, etc).

// * BAYER-PRESERVING LINE-INTERLEAVING BEHAVIOR *
localparam integer INTERLEAVE_JUMP_SIZE = 12'd2*N_INTERLEAVED_FRAMES - 12'd1;  // b/c average jump has to be N_INTERLEAVED_FRAMES: (1+x)/2 = n_interleave
localparam integer INTERLEAVED_LINES_PER_FRAME = LINES_IN_FRAME / N_INTERLEAVED_FRAMES; // floored if non-integral
reg [11:0] line_in_interleaved_frame = 12'b0; // (0-idx) What's the line number in the current interleaved frame
reg [4:0] interleaved_frame_index = 5'b0; // (0-idx) What interleaved frame are we on

// `target_interleaved_line_to_sample`: what is the next up (present or future)
// line in a frame should be sampled for the current/up-coming line of the
// interleaved frame.
//
// Since we have to do an equality test with `target_interleaved_line_to_sample`,
// there's a possibility that a SEU in it could cause its value to shoot to a
// non-existant line, meaning we'd halt since we'd never reach that line number.
// So, we'll use 3-way consensus on this value in order to make sure we know
// what it is in the case of a poorly placed SEU.
// The delay here isn't an issue since we set it at the beginning of the
// post-line blanking interval and don't check it until the following line.
reg [11:0] target_interleaved_line_to_sample__input [0:2];
// Wire up consensus circuit:
wire [11:0] target_interleaved_line_to_sample = (
      (target_interleaved_line_to_sample__input[2] & target_interleaved_line_to_sample__input[1])
    | (target_interleaved_line_to_sample__input[2] & target_interleaved_line_to_sample__input[0])
    | (target_interleaved_line_to_sample__input[1] & target_interleaved_line_to_sample__input[0])
);

reg all_interleaved_frames_complete = 1'b0; // signal that we've sampled everything:
always @(posedge clk) begin
    if(not_streaming) begin
        // For extra timing margin, reset all signals and registers while we're
        // not streaming (before a stream even starts):
        all_interleaved_frames_complete = 1'b0;
        interleaved_frame_index = 1'b0;
        line_in_interleaved_frame = 12'b0;
        // Set all consensus inputs for target:
        target_interleaved_line_to_sample__input[0] = 12'b0;
        target_interleaved_line_to_sample__input[1] = 12'b0;
        target_interleaved_line_to_sample__input[2] = 12'b0;
    end else if(
        line_counter_incremented_at_end
        && (prev_line_in_frame == target_interleaved_line_to_sample)
    ) begin
        // Line counter just incremented after a line ended and that line was
        // part of the current interleaved frame.
        //
        // (NOTE: we don't want to just catch the line end signal because the
        // line incrementer will also be looking for that signal and we can't
        // be sure which side we'll land on. Better to just make sure we go
        // after it.)

        // Check if that line was the last line in the interleaved frame:
        if(line_in_interleaved_frame >= (INTERLEAVED_LINES_PER_FRAME-1'b1)) begin // >= not == to help recover from SEU
            // We're done with this interleaved frame:
            line_in_interleaved_frame = 1'b0;
            // Move on to the next frame or signal that we're done:
            if(interleaved_frame_index >= (N_INTERLEAVED_FRAMES-1'b1)) begin // >= not == to help recover from SEU
                // We just finished the final frame, signal that we're done
                // with all frames:
                all_interleaved_frames_complete = 1'b1;
            end else begin
                // There're more frames. Move on to the next interleaved frame:
                interleaved_frame_index = interleaved_frame_index + 5'b1;
                // Start the target line back at the beginning
                // (skipping 2 lines for each interleaved frame to preserve
                // bayering):
                target_interleaved_line_to_sample__input[0] = 12'b2 * interleaved_frame_index;
                target_interleaved_line_to_sample__input[1] = target_interleaved_line_to_sample__input[0];
                target_interleaved_line_to_sample__input[2] = target_interleaved_line_to_sample__input[0];
            end
        end else begin
            // We *didn't* just finish the last line in this interleaved
            // frame, so just increment to the next line:
            line_in_interleaved_frame = line_in_interleaved_frame + 12'b1;

            // ... and Update the target line to capture
            // (knowing that we're not at the end):
            //
            // Instead of accomplishing an 8-frame interleave by skipping 7,
            // this reads 2, skips 14 so bayering data is preserved, leaving
            // the door open for possible color extraction. Still only reads 
            // 2/16=1/8 of the data per frame, so the total relative I/O ratio
            // for the FIFO will be the same.
            // NB: (1+15)/2 = 8
            if(target_interleaved_line_to_sample[0] == 1'b0) begin
                target_interleaved_line_to_sample__input[0] = target_interleaved_line_to_sample + 12'd1;
                target_interleaved_line_to_sample__input[1] = target_interleaved_line_to_sample__input[0];
                target_interleaved_line_to_sample__input[2] = target_interleaved_line_to_sample__input[0];
            end else if(target_interleaved_line_to_sample < (LINES_IN_FRAME - INTERLEAVE_JUMP_SIZE)) begin
                target_interleaved_line_to_sample__input[0] = target_interleaved_line_to_sample + INTERLEAVE_JUMP_SIZE;
                target_interleaved_line_to_sample__input[1] = target_interleaved_line_to_sample__input[0];
                target_interleaved_line_to_sample__input[2] = target_interleaved_line_to_sample__input[0];
            end else begin // (target_interleaved_line_to_sample + INTERLEAVE_JUMP_SIZE) >= LINES_IN_FRAME
                // Lands us outside of the set of valid lines
                //
                // This means 2 does not go evenly into `INTERLEAVED_LINES_PER_FRAME` (e.g. 243/2),
                // and, as such, the final pair of lines gets cut in half,
                // meaning after half of the interleaved frames have elapsed,
                // we'll be overrunning the boundary.
                // Instead, use this overrun to fill in lines we missed because 
                // they were cut off from the final line pairs of previous frames:
                //
                // e.g. if LINES_IN_FRAME is 1944, and N_INT is 8, INT_LINES_PER_FRAME is 243.
                // In this case, the final pair of lines in frame 0 would be
                // (1936,1937) but 1937 would be the 244th line so it would get cut off.
                // Later frame 4 would include half of an extra pair of lines: 1944,
                // so we can instead use this frame to sample 1937 that we missed earier.
                // In this example, without this accomodation, the final 3 lines of each
                // frame would be:
                // 0: 1920,1921, 1936   - 1937 cutoff
                // 1: 1922,1923, 1938   - 1939 cutoff
                // 2: 1924,1925, 1940   - 1941 cutoff
                // 3: 1926,1927, 1942   - 1943 cutoff
                // 4: 1928,1929, 1944   - 1944 doesn't exist (line nums are 0-indexed)
                // 5: 1930,1931, 1946   - 1946 doesn't exist 
                // 6: 1932,1933, 1948   - 1948 doesn't exist 
                // 7: 1934,1935, 1950   - 1950 doesn't exist
                // with this accommodation, we can fill in the missing lines and the final 3 lines would be:
                // 0: 1920,1921, 1936   - 1937 cutoff
                // 1: 1922,1923, 1938   - 1939 cutoff
                // 2: 1924,1925, 1940   - 1941 cutoff
                // 3: 1926,1927, 1942   - 1942 cutoff
                // 4: 1928,1929, 1937   - 1937 filled in
                // 5: 1930,1931, 1939   - 1939 filled in
                // 6: 1932,1933, 1941   - 1941 filled in
                // 7: 1934,1935, 1943   - 1943 filled in
                // This should work (if needed) so long as N_INTERLEAVED_PAIRS is a factor of LINES_IN_FRAME.
                target_interleaved_line_to_sample__input[0] = target_interleaved_line_to_sample + (INTERLEAVE_JUMP_SIZE - N_INTERLEAVED_FRAMES + 12'd1);
                target_interleaved_line_to_sample__input[1] = target_interleaved_line_to_sample__input[0];
                target_interleaved_line_to_sample__input[2] = target_interleaved_line_to_sample__input[0];
            end
        end
    end else if(target_interleaved_line_to_sample >= LINES_IN_FRAME) begin
        // Multiple poorly placed SEUs must have landed us here.
        // If we do nothing, we'll halt. We can't guess which line we should be
        // on so just move on so just end early.
        // Image will be cut short which should be a good indicator that we
        // need to reboot the FPGA.
        all_interleaved_frames_complete = 1'b1;
    end
end


// * FRAME COUNTER *
// Count of how many frames have elapsed since the start of streaming.
// For diagnostics. Rolls over at 255.
// We don't use this to control anything, so this can just count the number of
// frame start pulses without any additional sanity check logic.
reg [7:0] frame_number = 8'b0;
always @(posedge clk) begin
    if (not_streaming) begin
        frame_number = 8'b0;
    end else if(s_frame_start) begin
        frame_number = frame_number + 8'b1;
    end
end


// * PIXEL MUX *
ImageMetadataMux imageMetadataMux(
    .pixel_data(sync_PIXDATA[11:4]),
    .data_to_save(data_for_fifo),
    .pixel_index_in_line(pixels_elapsed_in_line), // serves as idx before and during pulse.
    .line_in_frame(line_in_frame), // (0-idx)
    .line_in_interleaved_frame(line_in_interleaved_frame), // (0-idx)
    .sensor_frame_number(frame_number), // what is the frame count from the sensor
    .interleaved_frame_number(interleaved_frame_index), // what is the index of the interleaved frame in the current image
    .imaging_mode(imaging_mode)
);


// * TRANSACTION FINITE STATE MACHINE *
// TRANSACTION FSM STATES (one-hot values):
localparam WAITING_FOR_COMMAND          = 4'h0; // Idle. No longer streaming bits into the FIFO.
localparam COMMANDED_WAITING_FOR_FRAME  = 4'h2; // We've been told to take a picture and are waiting for the start of the next frame to begin
localparam STREAMING_IMAGE              = 4'h4; // Loading bits into the FIFO actively.
reg [3:0] capture_state = WAITING_FOR_COMMAND;
reg [3:0] capture_state_prev = WAITING_FOR_COMMAND; // Capture state on the last clk cycle
wire streaming_started_pulse = (capture_state == STREAMING_IMAGE) && (capture_state_prev != STREAMING_IMAGE);  // 1 sys-clk pulse telling observers that the streaming just started
wire not_streaming = (capture_state != STREAMING_IMAGE);
// TODO: Watch out for pulse weirdness on camera switchover.
// -- make sure that doesn't happen while this is operating and make sure this
// won't be processing signals for a couple pulses after (maybe add a new CAM_SWITCHOVER state?)

// Keep Pixel: should we be sampling this pixel. Stays high as long as we're
// inside a valid line.
// NOTE: this is the bit to check when `sample_pixel_pulse` goes high telling
// us if we should keep this pixel (i.e. the FIFO write-enable)
// - line_num == target_line_to_sample  (<< make this a sample 2, skip 14 pattern so we can preserve bayering?)
// - line_active
// - SHOULD WE BE SAMPLING RN? (cap_frames)




// COMPUTE OUTPUT METASIGNALS:
// - Can we be loading pixels now:
// - LOAD DATA INTO FIFO
// - COMPOSITE IMAGE COMPLETE
// - DATA TO LOAD INTO FIFO (with header check)
//          -- should we make the header marker be color in bayer? (or at least a column of it)



load_pixels = line_complete
// what's the next line number we need. If we missed it in this frame somehow, just get it in the next frame (this is okay b/c we're assuming a static scene)

// Line complete:
- LV is low
- AND pixels in line >= 2592

// 


// Perform logical checks on FV and LV (LV must be at least X )


    // Grab important transitions:

    // Trigger a sampling of the data 2 sys clocks after pixclk falls:

    // Delay signals so they're aligned with the X'd data:

    // Pixel in line

    // * THINK ABOUT THE DELAYS
end

    // Use the synchronized signals

    // output all raw synced signals PLUS computed signals:
    // - PIX_SAMPLE (to drive FIFO) which comes 1 sys clk AFTER PIX_CLK fall
    // = COMPUTED LV which accounts for count since the last transition in EITHER direction - also resets to align with FV.
    // - same deal for FV?
    // - PV

    // do we want to also dejitter PIXCLK in here somewhere? 1:1 PLL at I or O? (probably O?) ... would we need to resync
    // sync and dejitter external clock

    // also counts for pixel in line, line in frame, line in interleave
    // and then feed this into a MUX for data vs metadata header (sync sample pulse with that too?)

    // make sure this will resync on each line

    // put all signals through a 3 or 4 sysclk wide averaging window?
    // -- 4 is too big. only get 4 or 5 sysclks per pixclk edge

end


endmodule

