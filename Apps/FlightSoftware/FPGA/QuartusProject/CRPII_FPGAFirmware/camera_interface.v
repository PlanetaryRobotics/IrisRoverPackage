// FSM and support logic for interfacing with the Camera Sensors

// Store settings per image (mode etc) - don't apply changes until capturing a new image.

module camera_interface
#(
    parameter SYNC_STAGES = 10, // Number of input synchronization stages    
    parameter PIX_SAMPLING_DELAY = 3, // Number of system clocks to wait after PIX_CLK falls before sampling the pixel data (if you want this to be 0, you need to remove the associated delay stage - setting this to 0 won't work).
    parameter VALID_LINE_SIZE = 12'd2592 // Number of valid pixels in a line of a frame
)
(
    input wire clk,
    input wire FV,
    input wire LV,
    input wire PIXCLK,
    input wire [11:0] PIXDATA,
    input wire FIFO_WR_REQ  // Signal used to enter data into the FIFO,

    input wire request_image,
    output wire image_started,
    output wire image_finished
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

// SIGNAL SYNCHRONIZATION AND CONDITIONING:
// Enabled by the fact that we get 4 full system clocks per PIXCLK and 8 between each rise.
integer i; // not real, just in the assembler
// Synchronizer data:
reg [14:0] sync [0:SYNC_STAGES]; // 12b data + 1 bit clk, 1 bit FV, 1 bit LV = 15b
wire [14:0] sync_out = sync[SYNC_STAGES];
// Data buffer:
reg [14:0] sync_buff [0:2];
// Indices in the sync busses (PIXDATA will always be 11:0):
localparam IDX_PIXCLK = 14;
localparam IDX_FV = 13;
localparam IDX_LV = 12;
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


// Perform "Averaging":
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


// Edge detectors:
// Detect edges on the synchronized bus outputs:
wire [14:0] sync_posedge = sync_buff[2] & ~sync_buff[1];
wire [14:0] sync_negedge = ~sync_buff[2] & sync_buff[1];
// Detect edges on the averaged bus outputs:
wire [14:0] sync_avg_posedge = sync_avg & ~sync_avg_pre;
wire [14:0] sync_avg_negedge = ~sync_avg & sync_avg_pre;

// Extract important data:
wire [11:0] sync_PIXDATA = sync_avg[11:0];
wire sync_LV = sync_avg[IDX_LV];
wire sync_FV = sync_avg[IDX_FV];

// Grab important transitions:
wire s_pixclk_fell = sync_avg_negedge[IDX_PIXCLK]; // a 1 sys-clock long pulse
wire s_line_start = sync_avg_posedge[IDX_LV]; // a 1 sys-clock long pulse
wire s_line_end = sync_avg_negedge[IDX_LV]; // a 1 sys-clock long pulse
wire s_frame_start = sync_avg_posedge[IDX_FV]; // a 1 sys-clock long pulse
wire s_frame_end = sync_avg_negedge[IDX_FV]; // a 1 sys-clock long pulse
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


// COMPUTE OUTPUT METASIGNALS:
// - Can we be loading pixels now:
// - LOAD DATA INTO FIFO
// - COMPOSITE IMAGE COMPLETE
// - DATA TO LOAD INTO FIFO (with header check)

// Track 

// * LINE LOGIC *
// Track line state (treat it as a tiny FSM instead of just combinatorial logic):
reg line_active = 0; // does the pixel data represent data inside a line
wire line_complete = ~line_active;
reg [11:0] pixels_elapsed_in_line = 0; // how many pixels have could we *sampled* in this line since it started (ignoring whether we're actually sampling this line)
always @(posedge clk) begin
    if (line_active) begin
        // Don't transition out of a line until the appropriate number of
        // pixels have elapsed (this helps ensure we always write a full line
        // of consecutive pixels):
        if(pixels_elapsed_in_line >= VALID_LINE_SIZE) begin
            line_active = 1'b0; // immediate
            pixels_elapsed_in_line <= 12'b0; // reset the count
            // Determine the next target line:
            // ! TODO: (WORKING-HERE)
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

// * LINE COUNTER *:
reg [11:0] line_in_frame = 0; // What line number are we in the frame (0-idx)
wire in_blanking_region = sync_PIXDATA[11:0] == 12'b0;
always @(posedge clk) begin
    // NOTE: `line_complete` is involved in the following checks in order to 
    // prevent the line number from being incremented until we've read a full
    // line's worth of pixels into the FIFO (meaning we won't suddenly jump
    // lines)
    
    // Increment the count on the line END during the blanking interval
    // because there are other signals we can cross check to make sure this
    // isn't errant. Also gives us more time to execute the increment logic
    // (the duration of the blanking interval).
    if( // Line is over when:
        !sync_LV // LV is low
        && line_complete // line is no longer active (we've read enough pixels)
        && in_blanking_region // all of the data bits are 0 (we're in blanking)
    ) begin
        line_in_frame = line_in_frame + 12'b1;
    end

    // ! TODO: (WORKING-HERE), first this. make sure this is done.

    // use a flag to make sure this doesn't happen again until syncLV

    // reset on new frame:

    // Only reset during the blanking region between frames:
    // (this is a long period rel. to sys clk and we don't need to act fast on
    // it so its okay to have a lot of combinatorial logic involved):
    if(
        !sync_FV // FV is low
        && !sync_LV // LV is low
        && in_blanking_region
        && line_complete // we've read all the data we need to read so it's okay to change this
    ) begin
        line_in_frame = 12'b0;
    end

    if(s_line_start) begin
        
    end
end


// * FRAME COUNTER *
// Count of how many frames have elapsed. For diagnostics. Rolls over at 255.
// We don't use this to control anything, so this can just count the number of
// frame start pulses without any additional sanity check logic.
reg [7:0] frame_number = 0;
always @(posedge clk) begin
    if(s_frame_start) begin
        frame_number = frame_number + 8'b1;
    end
end


// * TRANSACTION FINITE STATE MACHINE *
// TRANSACTION FSM STATES (one-hot values):
localparam WAITING_FOR_COMMAND          = 4'h0; // Idle. No longer streaming bits into the FIFO.
localparam COMMANDED_WAITING_FOR_FRAME  = 4'h2; // We've been told to take a picture and are waiting for the start of the next frame to begin
localparam STREAMING_IMAGE              = 4'h4; // Loading bits into the FIFO actively.


// Keep Pixel: should we be sampling this pixel. Stays high as long as we're
// inside a valid line.
// NOTE: this is the bit to check when `sample_pixel_pulse` goes high telling
// us if we should keep this pixel (i.e. the FIFO write-enable)
// - line_num == target_line_to_sample  (<< make this a sample 2, skip 14 pattern so we can preserve bayering?)
// - line_active
// - SHOULD WE BE SAMPLING RN? (cap_frames)


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

