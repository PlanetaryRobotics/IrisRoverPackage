module FPGA_Camera_Interface(input sysClk,
                             /* From Main Control Block */
                             input[7:0] I2C_Byte,
                             input[1:0] compression,
                             input      RGB,
                             input      cam_id,
                             input[27:0] timestamp,
                             input       trigger,
                             input[15:0] trigger_index,
                             input       hard_reset,
                             input      MCB_input_valid,

                            /*From Camera 0 */
                            inout I2C_Data_0, //to Camera 0 as well
                            input[11:0] pixel_data_0,
                            input pixel_clk_0,
                            input FV_0,
                            input LV_0,
                            input Strobe_0,

                            /*From Camera 1*/
                            inout I2C_Data_1, //to Camera 1 as well
                            input[11:0] pixel_data_1,
                            input pixel_clk_1,
                            input FV_1,
                            input LV_1,
                            input Strobe_1,

                            /*To Main Control Block */
                            output ready_for_next_byte,
                            output camera_id_error_flag,
                            output camera_timeout_error_flag,
                            output image_capture_failure_flag,
                            output camera_not_detected_flag,
                            output error_flag_valid,

                            /*To Camera 0 */
                            output I2C_clk_0,
                            output trigger_0,
                            output reset_0,

                            /*To Camera 1 */
                            output I2C_clk_1,
                            output trigger_1,
                            output reset_1,

                            /*To JPEG Block */
                            output[11:0] image_pixel_data,
                            output image_data_valid,
                            output image_metadata_valid
                              );




endmodule
