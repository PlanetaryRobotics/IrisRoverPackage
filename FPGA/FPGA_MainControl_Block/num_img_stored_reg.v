module num_img_stored_reg(input nav_img_added,
									input science_img_added,

								  input        start_flush_numimg_reg,

								  output reg[31:0] out_numimg_reg,
								  output	reg	   out_valid_numimg_reg
								  );


			reg [15:0] num_science_img_buff;
			reg [15:0] num_nav_img_buff;

			always@(*) begin
				if(nav_img_added) begin
					num_nav_img_buff = num_nav_img_buff + 1;
				end
				else if(science_img_added) begin
					num_science_img_buff = num_science_img_buff + 1;
				end

				if(start_flush_numimg_reg)begin
					out_numimg_reg = {num_nav_img_buff, num_science_img_buff};
					out_valid_numimg_reg = 1;
				end
				else begin
					out_valid_numimg_reg = 0;
				end
			end


endmodule
