module camera_output_thrower(input hard_reset_in,
														 input trigger_in,

														 output hard_reset_out,
														 output trigger_out
									);
					assign hard_reset_out = hard_reset_in;
					assign trigger_out = trigger_in;				
endmodule
