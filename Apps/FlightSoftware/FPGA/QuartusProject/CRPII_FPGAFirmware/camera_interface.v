// FSM and support logic for interfacing with the Camera Sensors.
//
// Designed with input synchronization and filtering, and line states to help 
// with immunity to signal noise and phase shifts on the incoming parallel bus.
// Also designed with some protections against SEU in mind.
//
// The core philosophy to the following implementation is that we care VERY much
// about getting every pixel without corruption and getting it in the right
// order, even with very noisy input signals.
// We care close to zero about how long it takes to capture (within reason) and
// also zero about motion blur (our environment is assumed to be static).
// So, all key input signals are tracked statefully and cross-referenced so we
// can be as sure as possible about staying in a state or transitioning.
//
// TODO: Check for FIFO full and don't advance internal state (which pixels and
//       lines are done) until we've actually written the data. (check for
//       almost_full too?)
// TODO: do we want to also dejitter PIXCLK in here somewhere? 1:1 PLL at I or
//       O? (probably O?) ... would we need to resync
//
// TODO: Pixel-timing images (weave into MUX)

module CameraSensorInterface
#(
    // * Config Variables:
    parameter integer SYNC_STAGES = 'd10, // Number of input synchronization stages    
    parameter integer PIX_SAMPLING_DELAY = 'd2, // Number of system clocks to wait after PIX_CLK falls before sampling the pixel data (if you want this to be 0, you need to remove the associated delay stage - setting this to 0 won't work).
    parameter integer VALID_LINE_SIZE = 12'd2592, // Number of valid pixels in a line of a frame
    parameter integer LINES_IN_FRAME = 12'd1944, // Number of lines in each full frame
    parameter integer STATE_TIMEOUT_MINUTES = 5, // Max number of minutes we can stay in a state before timing out and resetting

    // Number of frames to interleave into each image (bigger = more relief for
    // Flash FSM & more frames in "video", smaller = less motion blur and
    // faster image capture time). Needs to be at least 1.
    // Max 32 (max for bits in `interleaved_frame_index`).
    // To perform correctly, it should be a factor of `LINES_IN_FRAME`.
    parameter integer N_INTERLEAVED_FRAMES = 12'd8
)
(
    input wire clk,  // FPGA system clock

    // Signals from camera sensors:
    // Camera 1 Interface
    output reg camera_1_reset_bar,
    input wire camera_1_pixel_clock,
    input wire camera_1_FV,
    input wire camera_1_LV,
    input wire [11:0] camera_1_pixel_in,

    // Camera 2 Interface
    output reg camera_2_reset_bar,
    input wire camera_2_pixel_clock,
    input wire camera_2_FV,
    input wire camera_2_LV,
    input wire [11:0] camera_2_pixel_in,

    // Context information:
    input wire desired_camera_idx,  // 1'b0 for Camera 1, 1'b1 for Camera 2
    input wire [7:0] imaging_mode,

    // I/O control & status signals:
    input wire  request_image,  // Outside agent would like us to take an image as soon as possible
    output wire image_started,  // We've now started taking that image
    output wire image_finished,  // That image is done

    // FIFO Interface:
    // `data_for_fifo`: Data to be saved into the FIFO (pixel data or header info)
    output wire [7:0] data_for_fifo,
    // `sample_pixel_pulse`: Pulse goes high when it's time to sample a pixel
    // from `data_for_fifo`
    output wire sample_pixel_pulse,
    // `keep_pixel`: Whether the pixel data should be stored in the FIFO
    // (i.e. is it valid data). Stays high as long as we're in a valid region
    // of the image.
    // Wired to `wr_req` of FIFO.
    output wire keep_pixel
);

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
    for(i = 1; i<=SYNC_STAGES; i=i+1) begin
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
wire [14:0] sync_avg = (  // this signal is a pre-stage for edge-detection
      (sync_buff[2] & sync_buff[1])
    | (sync_buff[2] & sync_buff[0])
    | (sync_buff[1] & sync_buff[0])
);
reg [14:0] sync_avg_prev;
always @(posedge clk) begin
    // Add 1 stage of delay to averaged signals to check for edges in the
    // averaged signals:
    // (we have `sync_avg` be the later of these two stages so the `sync_avg`
    // edge detectors in time with the `sync_avg` signal)
    sync_avg_prev <= sync_avg;
end


// * Edge Detectors *
// Detect edges on the synchronized bus outputs:
wire [14:0] sync_posedge = sync_out[2] & ~sync_buff[0];
wire [14:0] sync_negedge = ~sync_out[2] & sync_buff[0];
// Detect edges on the averaged bus outputs:
wire [14:0] sync_avg_posedge = sync_avg & ~sync_avg_prev;
wire [14:0] sync_avg_negedge = ~sync_avg & sync_avg_prev;

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
assign sample_pixel_pulse = pix_sample_delay_pipe[PIX_SAMPLING_DELAY-1];
integer j;
always @(posedge clk) begin
    // Trigger a reading the pixels PIX_SAMPLING_DELAY system clocks after
    // pixel clock falls (give them time to finish transitioning -
    // accounts for any phase differences built up).
    // Data will stick around for about 8 sys clocks before transitioning so we can
    // safely make this a few wide.
    // NOTE: HOWEVER, we'll only have 4 sys clocks after PIXCLK falls guaranteed
    // before the data starts to transitions. Fewer if there's negative phase
    // shift (though there would have to be 20ns of negative shift for it to
    // matter 0 - the max amount spec'd at the sensor side (tPD) is 3.9ns when
    // PIXCLK is at its default 96MHz).
    // NOTE: Data transitions at PIXCLK rise, so there's already some delay
    // baked in.
    pix_sample_delay_pipe[0] <= s_pixclk_fell;
    for(j=1; j<PIX_SAMPLING_DELAY; j=j+1) begin
        pix_sample_delay_pipe[j] <= pix_sample_delay_pipe[j-1];
    end
end



// ***************************
// * FRAME TRACKING **********
// ***************************
// Tracks our position in a frame statefully.
// This data is used by the later Frame Processing section but not directly
// involved in command & control loop.


// * LINE LOGIC & PIXEL COUNTER *
// Track line state (treat it as a tiny FSM instead of just combinatorial logic):
reg line_active = 1'b0; // does the pixel data represent data inside a line
wire line_complete = ~line_active;
// `pixels_elapsed_in_line`: how many pixels have could we *sampled* in this
// line since it started (ignoring whether we're actually sampling this line)
// Starts at 0 (*before* we've sampled any pixels).
// Counts up to `VALID_LINE_SIZE` (*after* we've sampled the last pixel in the line)
// Thus:
//  - before and during `sample_pixel_pulse`, this serves as the vector index
//    of the pixel being sampled.
//  - after `sample_pixel_pulse`, this serves as a count of the number of
//    pixels we've pushed into the FIFO.

reg [11:0] pixels_elapsed_in_line = 12'b0;
reg waiting_for_end_of_line = 1'b0;
always @(posedge clk) begin
    if(camera_selection_in_progress) begin
        // Input signals may be changing over between cameras. We can't trust
        // edges we may observe. When we come out of this we'll be in a new
        // inter-frame blanking interval.
        // For now, just hold states and counts in reset:
        pixels_elapsed_in_line <= 12'b0;
        line_active <= 1'b0;
        waiting_for_end_of_line <= 1'b0;
    end else if (line_active) begin
        // Don't transition out of a line until the appropriate number of
        // pixels have elapsed (this helps ensure we always write a full line
        // of consecutive pixels):
        if(pixels_elapsed_in_line >= VALID_LINE_SIZE) begin  // >= not == to help recover from SEU
            line_active <= 1'b0;
            pixels_elapsed_in_line <= 12'b0; // reset the count
            waiting_for_end_of_line <= 1'b1; // Don't let line go active again until we exit this line.
        end else if(sample_pixel_pulse) begin
            // Set this using a "non-blocking" assignment so the value doesn't take
            // place until the next sys-clock (since we won't have actually sampled
            // it until then).
            pixels_elapsed_in_line <= pixels_elapsed_in_line + 12'b1;
        end
    end else begin // line is not active
        if(!sync_LV && in_blanking_region) begin
            waiting_for_end_of_line <= 1'b0;
        end
        if(sync_LV && !waiting_for_end_of_line) begin
            // We're not in the line active state but the flag is now high.
            line_active <= 1'b1;
        end
    end
end


// * LINE COUNTER *
// NOTE: As a matter of uniform practice,
// ALL LINE COUNTS ARE ADVANCED DURING THE BLANKING INTERVAL FOLLOWING A
// LINE'S COMPLETION (more time to execute the logic and be sure about the inc)
reg [11:0] line_in_frame = 12'b0; // What line number are we in the frame (0-idx)
reg [11:0] prev_line_in_frame = 12'b0; // What was the last line in the frame (0-idx). Typ. l_i_f-1, except when we start and when we wrap around.
reg line_counter_can_be_incremented = 1'b0;
reg line_counter_incremented_at_end = 1'b0; // 1 sys-clock pulse letting observers know we just incremented the line_counter (at the end of a line)
reg frame_ended_pulse = 1'b0; // 1 sys-clock wide pulse telling observers the last line of the current frame is definitely done. +
wire in_blanking_region = (sync_PIXDATA[11:0] == 12'b0);
// `out_of_frame`: Logic to check that we're DEFINITELY outside of a frame
// (in the inter-frame blanking interval)
// -- based on inputs only, not state tracking (pixel counts, etc):
// This signal has much higher susceptability to noise in exchange fo
// much higher certainty that if we see it, we're out of frame. Given that this
// state should be active for at least 50 consecutive PIXCLKs and we need it to
// be active for 1 sys clock to trigger `out_of_frame` checks, this is an
// acceptable and ideal trade-off.
wire out_of_frame = (
    !sync_FV // FV is low
    && !sync_LV // LV is low
    && in_blanking_region
);
always @(posedge clk) begin
    // NOTE: `line_complete` is involved in the following checks in order to 
    // prevent the line number from being incremented until we've read a full
    // line's worth of pixels into the FIFO (meaning we won't suddenly jump
    // lines)

    // Pulse resets:
    if(line_counter_incremented_at_end == 1'b1) begin
        // only let this persist for 1 sys-clock
        line_counter_incremented_at_end <= 1'b0;
    end

    if(frame_ended_pulse == 1'b1) begin
        frame_ended_pulse <= 1'b0;
    end

    // Actual camera-input-dependent logic:
    if(camera_selection_in_progress) begin
        // Input signals may be changing over between cameras. We can't trust
        // edges we may observe. When we come out of this we'll be in a new
        // inter-frame blanking interval.
        // For now, just hold states and counts in reset:
        line_in_frame <= 12'b0;
        prev_line_in_frame <= 12'b0;
        line_counter_can_be_incremented <= 1'b0;
        line_counter_incremented_at_end <= 1'b0;
        frame_ended_pulse <= 1'b0;
    end else begin

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
            line_counter_can_be_incremented <= 1'b0;
        
            prev_line_in_frame <= line_in_frame;
            line_in_frame <= line_in_frame + 12'b1;

            line_counter_incremented_at_end <= 1'b1; // trigger this after increment
        end

        if(line_active && !line_counter_can_be_incremented) begin  // NB: line_complete = ~line_active
            // Now that we're back in a line, allow the line counter to be
            // incremented again once we're out:
            line_counter_can_be_incremented <= 1'b1;
        end

        // Only reset during the blanking region between frames:
        // (this is a long period rel. to sys clk and we don't need to act fast on
        // it so its okay to have a lot of combinatorial logic involved):
        if(
            out_of_frame     // all inputs show that we're in an inter-frame blanking interval
            && line_complete // we've read all the data we need to read so it's okay to change this
        ) begin
            prev_line_in_frame <= line_in_frame;
            line_in_frame <= 12'b0;
            frame_ended_pulse <= 1'b1; // send after resetting counts
        end
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
        all_interleaved_frames_complete <= 1'b0;
        line_in_interleaved_frame <= 12'b0;
        interleaved_frame_index <= 5'b0;
        // Set all consensus inputs for target:
        target_interleaved_line_to_sample__input[0] <= 12'b0;
        target_interleaved_line_to_sample__input[1] <= 12'b0;
        target_interleaved_line_to_sample__input[2] <= 12'b0;
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
            line_in_interleaved_frame <= 1'b0;
            // Move on to the next frame or signal that we're done:
            if(interleaved_frame_index >= (N_INTERLEAVED_FRAMES-1'b1)) begin // >= not == to help recover from SEU
                // We just finished the final frame, signal that we're done
                // with all frames:
                all_interleaved_frames_complete <= 1'b1;
            end else begin
                // There're more frames. Move on to the next interleaved frame:
                interleaved_frame_index <= interleaved_frame_index + 5'b1;
                // Start the target line back at the beginning
                // (skipping 2 lines for each interleaved frame to preserve
                // bayering):
                target_interleaved_line_to_sample__input[0] <= interleaved_frame_index + interleaved_frame_index + 5'b2;
                target_interleaved_line_to_sample__input[1] <= interleaved_frame_index + interleaved_frame_index + 5'b2;
                target_interleaved_line_to_sample__input[2] <= interleaved_frame_index + interleaved_frame_index + 5'b2;
            end
        end else begin
            // We *didn't* just finish the last line in this interleaved
            // frame, so just increment to the next line:
            line_in_interleaved_frame <= line_in_interleaved_frame + 12'b1;

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
                target_interleaved_line_to_sample__input[0] <= target_interleaved_line_to_sample + 12'd1;
                target_interleaved_line_to_sample__input[1] <= target_interleaved_line_to_sample + 12'd1;
                target_interleaved_line_to_sample__input[2] <= target_interleaved_line_to_sample + 12'd1;
            end else if(target_interleaved_line_to_sample < (LINES_IN_FRAME - INTERLEAVE_JUMP_SIZE)) begin
                target_interleaved_line_to_sample__input[0] <= target_interleaved_line_to_sample + INTERLEAVE_JUMP_SIZE;
                target_interleaved_line_to_sample__input[1] <= target_interleaved_line_to_sample + INTERLEAVE_JUMP_SIZE;
                target_interleaved_line_to_sample__input[2] <= target_interleaved_line_to_sample + INTERLEAVE_JUMP_SIZE;
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
                target_interleaved_line_to_sample__input[0] <= target_interleaved_line_to_sample + (INTERLEAVE_JUMP_SIZE - N_INTERLEAVED_FRAMES + 12'd1);
                target_interleaved_line_to_sample__input[1] <= target_interleaved_line_to_sample + (INTERLEAVE_JUMP_SIZE - N_INTERLEAVED_FRAMES + 12'd1);
                target_interleaved_line_to_sample__input[2] <= target_interleaved_line_to_sample + (INTERLEAVE_JUMP_SIZE - N_INTERLEAVED_FRAMES + 12'd1);
            end
        end
    end else if(target_interleaved_line_to_sample >= LINES_IN_FRAME) begin
        // Multiple poorly placed SEUs must have landed us here.
        // If we do nothing, we'll halt. We can't guess which line we should be
        // on so just move on so just end early.
        // Image will be cut short which should be a good indicator that we
        // need to reboot the FPGA.
        all_interleaved_frames_complete <= 1'b1;
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
        frame_number <= 8'b0;
    end else if(s_frame_start) begin
        frame_number <= frame_number + 8'b1;
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


// * VALID PIXEL CHECK *
// Keep Pixel: should we be sampling this pixel. Stays high as long as we're
// inside a valid line.
// NOTE: this is the bit to check when `sample_pixel_pulse` goes high telling
// us if we should keep this pixel (i.e. the FIFO write-enable)
assign keep_pixel = (
    line_active                                                 // Are we in a line?
    && (line_in_frame == target_interleaved_line_to_sample)     // Is it the right line for the current interleave pos?
    && actively_streaming                                       // Should we be saving data rn at all?
);



// *********************************************
// * TRANSACTION FINITE STATE MACHINE **********
// *********************************************
// Controls state of image capture and transactions with outside agents who
// want us to take an image.
// Built according to Intel Quartus' standards for an FSM in Verilog-2001,
// to help with automatic state machine recognition.

// * TRANSACTION FSM STATES *
localparam BOOT                                 = 4'd0,
           IDLE                                 = 4'd1,
           SWITCHING_CAM__RST                   = 4'd2,
           SWITCHING_CAM__POST_SWITCHOVER_WAIT  = 4'd3,
           SWITCHING_CAM__UNRST                 = 4'd4,
           SWITCHING_CAM__WAIT_FOR_FRAME_START  = 4'd5,
           SWITCHING_CAM__WAIT_FOR_FRAME_END    = 4'd6,
           COMMANDED__WAITING_FOR_FRAME_END     = 4'd7,
           STREAMING_IMAGE                      = 4'd8;
// Declare the state register to be "safe" to implement
// a safe state machine that can recover gracefully from
// an illegal state (by returning to the reset state).
(* syn_encoding = "safe" *) reg [3:0] capture_state = BOOT;
reg [3:0] capture_state_prev_clk = BOOT; // Capture state on the last clk cycle
reg [3:0] capture_state_next = BOOT; // Capture state to transition to on the next cycle

// * Flags for Key States & State Transitions *
wire not_streaming = (capture_state != STREAMING_IMAGE);
wire actively_streaming = (capture_state == STREAMING_IMAGE);

// * Feedback Signals to Controller (agent who requested image) *
assign image_started = actively_streaming;
assign image_finished = not_streaming;

// * State Timer *
reg [36:0] state_timer = 37'b0; // 37b counter of how long we've been in the current state, capable of counting up to 30 minutes (9e10 cycles at 50MHz), way longer than any state should take.
localparam ONE_SECOND = 37'd50_000_000; // Num. cycles in a second (used in a lot of places)
localparam STATE_TIMEOUT_MAX_CYCLES = ONE_SECOND * 37'd60 * STATE_TIMEOUT_MINUTES;

// * Derived Camera Signals (MUX) *
reg no_camera_selected = 1'b1; // Whether any cameras have been selected (properly, through the FSM)
reg selected_camera_idx = 1'b0; // Which camera is currently selected ('b0 for Cam 1, 'b1 for Cam 2)
// MUX'd input signals from sensors (assigned to no sensor if we have nothing selected):
wire        FV          = no_camera_selected ? 1'b0             : (selected_camera_idx ? camera_2_FV : camera_1_FV);
wire        LV          = no_camera_selected ? 1'b0             : (selected_camera_idx ? camera_2_LV : camera_1_LV);
wire        PIXCLK      = no_camera_selected ? 1'b0             : (selected_camera_idx ? camera_2_pixel_clock : camera_1_pixel_clock);
wire [11:0] PIXDATA     = no_camera_selected ? {8'hCC, 4'b0}    : (selected_camera_idx ? camera_2_pixel_in : camera_1_pixel_in);  // Put fixed hex in upper 8 bits (where we read). Not 0 just so we can debug if something goes weird


// * State Machine Transition Logic*
// State machine logic impl. as combo per Intel recommendation for Quartus
// auto-detection.
reg camera_selection_in_progress = 1'b0; // Flag to let observers know that the camera is actively being switched and to not track any transitions the camera signals may be doing.
// * State Transitioner & Timer *
always @(posedge clk) begin
    if(capture_state != capture_state_next) begin
        state_timer <= 37'b0;  // immediately reset timer, then update state:
        // Advance to next state:
        capture_state <= capture_state_next;
    end else begin
        state_timer <= state_timer + 37'b1;
    end

    // Always update `capture_state_prev_clk` b/c, by definition,
    // it's always supposed to be a 1 clk-cycle shadow (1-FF shift reg) of
    // `capture_state`:
    capture_state_prev_clk <= capture_state; // note this is in parallel w/ the above

    if(state_timer > STATE_TIMEOUT_MAX_CYCLES) begin
        // If we've stayed in a state for WAY longer than should be the case,
        // we need to just yeet back to BOOT and start over.
        // This is the final line of defense against hangups.
        capture_state_next <= BOOT;
    end else begin
        case (capture_state)
            BOOT: begin
                // Hold both cameras in reset (active-low):
                camera_1_reset_bar <= 1'b0;
                camera_2_reset_bar <= 1'b0;
                no_camera_selected <= 1'b1;
                if(state_timer > ONE_SECOND) begin
                    // Things have had time to settle. Moving on to IDLE state.
                    // Not unresetting the cameras here b/c we do that through
                    // special states so we can control when it happens and react
                    // to it
                    capture_state_next <= IDLE;
                end
            end
            IDLE: begin
                // Idle. Not doing anything, incl. streaming bits into the FIFO.
                if(no_camera_selected || (selected_camera_idx != desired_camera_idx)) begin
                    // No camera selected (properly) yet or we need to change that.
                    // Only allowing this to happen in the idle state so we don't
                    // switch cameras while imaging.
                    camera_selection_in_progress <= 1'b1;
                    capture_state_next <= SWITCHING_CAM__RST;
                end else if(request_image) begin
                    capture_state_next <= COMMANDED__WAITING_FOR_FRAME_END;
                end
            end
            SWITCHING_CAM__RST: begin
                // Hold both cameras in reset for at least 1s (active-low):
                camera_1_reset_bar <= 1'b0;
                camera_2_reset_bar <= 1'b0;
                no_camera_selected <= 1'b1;  // Flag that we don't currently have *any* camera intentionally selected

                if(state_timer > ONE_SECOND) begin
                    // Perform the switch only after we've been in rst for a while,
                    // so switchover doesn't happen with the cameras active.
                    selected_camera_idx <= desired_camera_idx;
                    no_camera_selected <= 1'b0; // we've now selected a camera
                    capture_state_next <= SWITCHING_CAM__POST_SWITCHOVER_WAIT;
                end
            end
            SWITCHING_CAM__POST_SWITCHOVER_WAIT: begin
                // Wait here a bit, doing nothing, after the MUX switch before unresetting:
                // (makes sure the switchover completes while neither cam is active)
                if(state_timer > ONE_SECOND) begin
                    capture_state_next <= SWITCHING_CAM__UNRST;
                end
            end
            SWITCHING_CAM__UNRST: begin
                // Unreset JUST THE SELECTED CAMERA (active-low).
                // Leave the other off for power saving.
                if(selected_camera_idx == 1'b1) begin
                    camera_2_reset_bar <= 1'b1;
                end else begin
                    camera_1_reset_bar <= 1'b1;
                end

                // Wait a sec for things to start and stabilize
                // (longer than needed but that's fine).
                if(state_timer > ONE_SECOND) begin
                    capture_state_next <= SWITCHING_CAM__WAIT_FOR_FRAME_START;
                end
            end
            SWITCHING_CAM__WAIT_FOR_FRAME_START: begin
                // Let 1 frame elapse after camera is unreset before letting any 
                // state trackers start and make sure everything comes up during
                // an inter-frame boundary.
                // So, first we must wait for a frame start trigger...
                if(sync_FV) begin
                    capture_state_next <= SWITCHING_CAM__WAIT_FOR_FRAME_END;
                end
            end
            SWITCHING_CAM__WAIT_FOR_FRAME_END: begin
                // ... then we wait for the next inter-frame boundary.
                if(out_of_frame) begin
                    camera_selection_in_progress <= 1'b0;
                    capture_state_next <= IDLE;
                end
            end
            COMMANDED__WAITING_FOR_FRAME_END: begin
                // We've been told to take a picture.
                // Before we start streaming pixels, we need two things:
                // - to make sure we don't start capturing in the middle of a frame
                // - for the frame processing circuits to come online during an
                //   inter-frame blanking interval (so they have some delay to set
                //   up and catch the start transition) are waiting for the start
                //   of the *next* frame to begin.
                // So, all we need to do is wait until we're definitely in an
                // inter-frame blanking interval and we can proceed.
                if(out_of_frame) begin
                    capture_state_next <= STREAMING_IMAGE;
                end
            end
            STREAMING_IMAGE: begin
                // Actively streaming bytes into the FIFO.
                // This continues until the Frame Processing circuits tell us we've
                // captured all the lines we need for all of our interleaved frames:
                if(all_interleaved_frames_complete) begin
                    capture_state_next <= IDLE;
                end
            end
            default: begin
                // Shouldn't be here. Reboot circuit:
                capture_state_next <= BOOT;
            end
        endcase
    end
end

endmodule
