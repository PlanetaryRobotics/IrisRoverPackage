module flashmem_spi_interface(input sysClk,
                              input[7:0] tx_byte,
                              input MISO,

                              output SPICLK,
                              output MOSI,
                              output CS,
                              output[7:0] rx_byte,
                              output next_byte_request_flag
                              );


endmodule
