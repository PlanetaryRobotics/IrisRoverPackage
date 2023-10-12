// Single source of truth for the current version of the FW
// (to be included in metadata).
module IrisFpgaFwVersion
# (
    // Change the version for the Iris Camera FPGA FW information here:
    parameter integer FW_VER_MAJ = 8'd11,    // Version: ++ when drafting a new Release Candidate (for a new upload opportunity)
    parameter integer FW_VER_MIN = 8'd1,     // Subversion: ++ when a new major feature has been added / made to work
    parameter integer FW_VER_PATCH = 8'd0    // Patch: ++ when you make a change and want to reflect that.
)   
(
    output wire [7:0] major,
    output wire [7:0] minor,
    output wire [7:0] patch
);
    assign major = FW_VER_MAJ;
    assign minor = FW_VER_MIN;
    assign patch = FW_VER_PATCH;
endmodule
