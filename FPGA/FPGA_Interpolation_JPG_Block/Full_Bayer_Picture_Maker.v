
module Full_Bayer_Picture_Maker(input sysClk,
																input[11:0] dozen_in,
																input pixeldata_flag,

																output[11:0] bayer_image[1943:0][2591:0],
																output valid_output
																);

			reg [11:0] bayer_image_s[1943:0][2591:0];
			reg valid_output_s;

			assign bayer_image = bayer_image_s;
			assign valid_output = valid_output_s;

			reg[11:0] column_counter_s;
			wire[11:0] column_counter;

			assign column_counter = column_counter_s;

			reg[10:0] row_counter_s;
			wire[10:0] row_counter;

			assign row_counter = row_counter_s;


			initial begin
				bayer_image_s = 0;
				valid_output_s = 0;
				column_counter_s = 0;
				row_counter_s = 0;
			end

			always@(posedge sysClk)begin
				if(pixeldata_flag)begin
					if(column_counter == 1943) begin
						column_counter_s <= 0;
						row_counter_s <= row_counter + 1;
					end
					else begin
						column_counter_s <= column_counter + 1;
						row_counter_s <= row_counter;
					end
				end

				if(row_counter == 2591)begin
					valid_output_s <= 1;
					row_counter_s <= 0;
					column_counter_s <= 0;
				end
				else begin
					valid_output_s <= 0;
					row_counter_s <= row_counter;
					column_counter_s <= column_counter;
				end
			end

			always@(posedge sysClk)begin
				if(pixeldata_flag)begin
					bayer_image_s[row_counter][column_counter] <= dozen_in;
				end
			end
endmodule
