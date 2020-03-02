module byte_buffer(input sysClk,

                   input[7:0] byte_from_jpg_block,
                   input byte_from_jpg_block_valid,

                  output[7:0] byte_for_flashmem_spi,
                  output byte_for_flashmem_spi_valid,

                  output navigation_image_added_flag,
                  output science_image_added_flag,

                  output[31:0] image_size,
                  output image_size_output_valid,
                  output[15:0] image_index,
                  output image_index_output_valid
                  );

endmodule
