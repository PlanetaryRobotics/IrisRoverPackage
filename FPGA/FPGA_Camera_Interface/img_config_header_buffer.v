module img_metadata_header_buffer(input      sysClk,
										  /*From input port connected to the Main Control Block */
										  input [1:0]  compression,
										  input        RGB,
										  input        cam_id,
										  input [27:0] timestamp,
										  input [15:0] trigger_index,
										  input 			valid_input,

										  /*once all the pixels of an image with this metadata goes through to the JPEG block, flush this metadata right after */
										  input all_pixels_in,

										  /*To output port connected to the JPG block */
										  output[11:0] dozen_out,
										  output dozen_out_valid
							);
							reg[11:0] dozen_out_s;
							reg  			dozen_out_valid_s;

							assign dozen_out = dozen_out_s;
							assign dozen_out_valid= dozen_out_valid_s;

							reg[47:0] full_metadata_buffer_r;
							wire[47:0] full_metadata_buffer;

							assign full_metadata_buffer = full_metadata_buffer_r;

							reg[2:0] all_pixels_in_buff_s;
							wire[2:0] all_pixels_in_buff;

							assign all_pixels_in_s = all_pixels_in;

							reg flushing_mode_s;
							wire flushing_mode;

							assign flushing_mode = flushing_mode_s;

							reg[1:0] dozen_counter_s;
							wire[1:0] dozen_counter;

							assign dozen_counter = dozen_counter_s;

							initial begin
								dozen_out_s = 0;
								dozen_out_valid_s = 0;
								full_metadata_buffer_r = 0;
								all_pixels_in_buff_s = 0;
								flushing_mode_s = 0;
								dozen_counter_s = 0;
							end

							assign full_metadata_buffer = full_metadata_buffer_r;
							always@(*) begin
									if(valid_input)begin
											full_metadata_buffer_r = {compression, RGB, cam_id, timestamp, trigger_index};
									end
									else begin
										full_metadata_buffer_r = full_metadata_buffer;
									end
							end


							always@(posedge sysClk)begin
								all_pixels_in_buff_s <= {all_pixels_in_buff[1:0], all_pixels_in};
								if(all_pixels_in_buff == 2'b01)begin
									flushing_mode_s <= 1;
								end
								if(dozen_counter == 2'b11)begin
									flushing_mode_s <= 0
								end
							end


							always@(posedge sysClk)begin
								if(flushing_mode)begin
									case(dozen_counter)
										2'b00: begin
												dozen_out_s <= full_metadata_buffer[47:36];
												dozen_out_valid_s <= 1;
										end
										2'b01: begin
												dozen_out_s <= full_metadata_buffer[35:24];
												dozen_out_valid_s <= 1;
										end
										2'b10: begin
												dozen_out_s <= full_metadata_buffer[23:12];
												dozen_out_valid_s <= 1;
										end
										2'b11: begin
												dozen_out_s <= full_metadata_buffer[11:0];
												dozen_out_valid_s <= 1;
										end
										default: begin
												dozen_out_s <= 12'h00;
												dozen_out_valid_s <= 0;
										end
									endcase

									dozen_counter_s <= dozen_counter + 2'b01;
								end
							end
endmodule
