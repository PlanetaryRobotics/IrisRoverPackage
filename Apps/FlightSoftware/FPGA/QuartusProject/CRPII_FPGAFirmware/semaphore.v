// Sets flag `flag` on the posedge of `trigger` if `condition`.
// Resets on the `posedge of `reset`.
module ConditionalSemaphore
(
    input wire trigger,
    input wire reset,
    input wire condition,
    output reg flag
)
    always@(posedge trigger or posedge reset) begin
        if(reset) begin
            flag = 1'b0;
        end
        else begin
            if(condition)
                flag = 1'b1;
        end
    end
endmodule

module Semaphore
(
    input wire trigger,
    input wire reset,
    input wire condition,
    output reg flag
)
    always@(posedge trigger or posedge reset) begin
        flag = reset ? 1'b0 : 1'b1;
    end
endmodule

// Example:
// ConditionalSemaphore semaphore( .flag( flash_WREN_done )
//     .trigger(flash_mem_interface_done), .reset(reset_flash_WREN_done),
//     .condition( current_state == flash_WREN )
// )
