//  MUX to determine what data actually enters the FIFO. This:
//  - Replaces first line of an interleaved image frame with metadata (and a
//    gradient to make it clearly intentional and unnatural)
//  - Replaces the start of each line with metadata about that line to help with
//    reconstruction in the event of corruption (and a gradient to make sure it
//    searchable in case of line feed bugs).
//
// Description & Rationale:
// The base case is that it's just the processed pixel data, however we'll
// co-opt the first pixels of every line for metadata as well as the first row
// of each interleaved frame.
//
// We *overwrite* the first few columns with line metadata instead of just
// increasing the line length and appending that data because the rest of the
// FSW pipeline (specifically, the flash interactions) are designed for line
// lengths of 2592. Since all of this rework is very late stage, minimizing
// additional changes is a plus.
// This line metadata serves two purposes:
// 1. Provide metadata about where that line comes from.
// 2. Allow GSW to scan for known pattern in the image data to brute-force
// detect the start of line data in the case of corruption (which we have seen).
//
// We also replace the first row of *each interleaved frame* in the image with
// file metadata since Hercules may only choose to downlink one frame of data
// and we need to make sure we still get the header. As above, that behavior
// could be changed so we always grab a metadata line whenever grabbing an
// image but, for now, we need to mimize external changes.
//
// A nice side effect of these metadata sections is they'll act as known-good
// test patterns embedded in the image.
module ImageMetadataMux
(
    input [7:0] pixel_data,
    output [7:0] data_to_save,  // data to be saved into the FIFO (pixel data or header info)
    input [11:0] pixel_index_in_line, // (0-idx)
    input [7:0] line_in_frame, // (0-idx)
    input [7:0] line_in_interleaved_frame, // (0-idx)
    input [7:0] sensor_frame_number, // what is the frame count from the sensor
    input [7:0] interleaved_frame_number, // what is the index of the interleaved frame in the current image
    input [7:0] imaging_mode
);
    // Grab version info:
    wire [7:0] FW_VER_MAJ, FW_VER_MIN, FW_VER_PATCH;
    IrisFpgaFwVersion fw_version(.major(FW_VER_MAJ), .minor(FW_VER_MIN), .patch(FW_VER_PATCH));

    // Initialize all the fixed text in the header (X) is just a placeholder at
    // indices that'll get filled in later.
    localparam integer FIXED_HEADER_TEXT_LEN = 'd84; // num bytes in fixed header
    localparam integer FIXED_HEADER_TEXT_MAX_IDX = FIXED_HEADER_TEXT_LEN * 'd8 - 'd1; // index of final bit in fixed header
    reg [FIXED_HEADER_TEXT_LEN*8-1:0] HEADER_FIXED_TEXT = "IRIS-FPGA-FW--RCX.X.X--SENSOR-FRAME-NUM:X--INTERLEAVED-FRAME-NUM:X--IMAGING-MODE:X--";
    // Indices to replace text with some other value:
    localparam integer IDX__FW_VER_MAJ = 'd16;
    localparam integer IDX__FW_VER_MIN = 'd18;
    localparam integer IDX__FW_VER_PATCH = 'd20;
    localparam integer IDX__SENSOR_FRAME_NUM = 'd40;
    localparam integer IDX__INTERLEAVED_FRAME_NUM = 'd65;
    localparam integer IDX__IMAGING_MODE = 'd81;

    // Default byte stream for metadata line is a gradient which wraps every 255 cols:
    // Skipping all the even pixels in an even line should make the gradient
    // appear red when debayered and be unaffected by the subsequent line's contents
    // (the start of each interleaved frame always corresponds with an even line in sensor-space).
    //
    // Side benefit of this approach is it lets us look for issues in the pixel counting process.
    wire [7:0] gradient = pixel_index_in_line[0] ? pixel_index_in_line[7:0] : 0;

    reg [7:0] header_byte;
    always @(*) begin // combinatorial logic
        if(pixel_index_in_line == IDX__FW_VER_MAJ) begin
            header_byte = FW_VER_MAJ;
        end else if(pixel_index_in_line == IDX__FW_VER_MIN) begin
            header_byte = FW_VER_MIN;
        end  else if(pixel_index_in_line == IDX__FW_VER_PATCH) begin
            header_byte = FW_VER_PATCH;
        end else if(pixel_index_in_line == IDX__SENSOR_FRAME_NUM) begin
            header_byte = sensor_frame_number;
        end else if(pixel_index_in_line == IDX__INTERLEAVED_FRAME_NUM) begin
            header_byte = interleaved_frame_number;
        end else if(pixel_index_in_line == IDX__IMAGING_MODE) begin
            header_byte = imaging_mode;
        end  else if(pixel_index_in_line < FIXED_HEADER_TEXT_LEN) begin
            // Not a special character but in the header string, so just index the string:
            header_byte = HEADER_FIXED_TEXT[FIXED_HEADER_TEXT_MAX_IDX-8*pixel_index_in_line -: 8];
        end else begin
            header_byte = gradient;
        end
    end


    // Add line header:
    // Base header is a gradient (so its clearly identifiable and intentional).
    // Every 4th byte is overridden with metadata for the line.
    // Stride is 3 because we still want a clearly identifiable pattern that's
    // very unlikely to happen in noise or a real image so we can brute force
    // search for a line header if line feed corruption occurs (as we've seen
    // before).
    // 00 serves as a placeholder for where we're inserting data.
    localparam integer FIXED_LINE_HEADER_LEN = 'd15;
    localparam integer FIXED_LINE_HEADER_MAX_IDX = FIXED_LINE_HEADER_LEN * 'd8 - 'd1; // index of final bit in fixed line header
    reg [FIXED_LINE_HEADER_LEN*8-1:0] FIXED_LINE_HEADER = 120'h11_22_33_00_55_66_77_00_99_AA_BB_00_DD_EE_FF;
    // Indices to replace text with some other value:
    localparam integer IDX_LINE__FRAME_NUM = 'h3;
    localparam integer IDX_LINE__FRAME_LINE_NUM = 'h7;
    localparam integer IDX_LINE__INTERLEAVED_FRAME_LINE_NUM = 'hB;

    reg [7:0] line_header_byte;
    always @(*) begin // combinatorial logic
        if(pixel_index_in_line == IDX_LINE__FRAME_NUM) begin
            // Showing the SENSOR frame number so we can see if any FVs have
            // happened during our interleaved frame (for diagnostics).
            line_header_byte = sensor_frame_number;
        end else if(pixel_index_in_line == IDX_LINE__FRAME_LINE_NUM) begin
            line_header_byte = line_in_frame;
        end else if(pixel_index_in_line == IDX_LINE__INTERLEAVED_FRAME_LINE_NUM) begin
            line_header_byte = line_in_interleaved_frame;
        end else begin
            line_header_byte = FIXED_LINE_HEADER[FIXED_LINE_HEADER_MAX_IDX-8*pixel_index_in_line -: 8];
        end
    end


    // MUX together the output:
    assign data_to_save = (line_in_interleaved_frame > 0) ? (pixel_index_in_line < FIXED_LINE_HEADER_LEN ? line_header_byte : pixel_data) :
                                                            header_byte;
endmodule
