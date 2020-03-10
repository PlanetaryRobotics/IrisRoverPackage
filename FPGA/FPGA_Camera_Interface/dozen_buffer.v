module dozen_buffer(input[11:0] dozen_in,
                    input       metadata_in_flag,
                    input       pixel_data_in_flag,

                    output[11:0] dozen_out,
                    output       metadata_out_flag,
                    output       pixel_data_out_flag);

        assign dozen_out = dozen_in;
        assign metadata_in_flag = metadata_out_flag;
        assign pixel_data_out_flag = pixel_data_out_flag;


endmodule
