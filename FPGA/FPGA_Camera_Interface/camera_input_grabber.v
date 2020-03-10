module camera_input_grabber(input 			 sysClk,
														input 			 trigger2_event_flag,
														input[11:0]  pixel_data_in,
														input 			 FV,
														input 			 LV,
														input 			 Strobe,

														output 			 img_cap_fail_flag,
														output[11:0] pixel_data_out,
														output 			 valid_out
									);

				reg img_cap_fail_flag_s;
				reg[11:0] pixel_data_out_s;
				reg valid_out_s;

				assign img_cap_fail_flag = img_cap_fail_flag_s;
				assign pixel_data_out = pixel_data_out_s;
				assign valid_out = valid_out_s

				reg[1:0] img_cap_timeout_counter_s;
				wire[1:0] img_cap_timeout_counter;

				assign img_cap_timeout_counter = img_cap_timeout_counter_s;

				reg[14:0] batch_1_s;
				wire[14:0] batch_1;

				assign batch_1 = batch_1_s;

				reg[14:0] batch_2_s;
				wire[14:0] batch_2;

				assign batch_2 = batch_2_s;

				reg[14:0] batch_3_s;
				wire[14:0] batch_3;

				assign batch_3 = batch_3_s;

				wire LV_in;
				wire FV_in;
				wire Strobe_in;

				assign FV_in = batch_3[2];
				assign LV_in = batch_3[1];
				assign Strobe_in = batch_3[0];


				initial begin
				  img_cap_timeout_counter_s <= 0;
					img_cap_fail_flag_s <= 0;
					pixel_data_out_s <= 0;
					valid_out_s <= 0;
					batch_1_s <= 0;
					batch_2_s <= 0;
					batch_3_s <= 0;
				end

				always@(posedge sysClk)begin
					batch_1_s <= {pixel_data_in, FV, LV, Strobe};
					batch_2_s <= batch_1;
					batch_3_s <= batch_2;
				end

				always@(posedge sysClk)begin
					if(FV_in & LV_in)begin
							pixel_data_out_s <= batch_3[14:3];
							valid_out_s <= 1;
					end
					else begin
						pixel_data_out_s <= 12'h00;
						valid_out_s <= 0;
					end
				end

				always@(posedge sysClk)begin
					if(trigger2_event_flag)begin
						img_cap_timeout_counter_s <= img_cap_timeout_counter + 1;
					end
					if(img_cap_timeout_counter == 2'b11)begin
						img_cap_timeout_counter_s <= 2'b00;
						img_cap_fail_flag_s <= 1;
					end
					else begin
						img_cap_fail_flag_s <= 0;
					end
				end
endmodule
