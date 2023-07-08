`include "cube_shifter.v"
`include "cube_control.v"

`define NUM_SHIFTERS 7

module cube_raster (
	output wire drq,
	output wire finished,
	output wire [`NUM_SHIFTERS-1:0] sout,
	input  wire clk,
	input  wire reset);

    wire [2:0] dp_addr;
    wire fifo0_load;
    wire fifo1_load;
    wire fifo0_en_load;
    wire clear_add;
    wire latch_add;
    wire data_en;
    wire data_zero;
    wire latch_shift;
    wire last_bit;
    wire one_bit;
    wire zero_bit;

    genvar i;
    generate for (i = 0; i < `NUM_SHIFTERS; i = i + 1) begin : slave_shifter
        cube_shifter cube_shifter (
            .sout(sout[i]),
            .clk(clk),
            .reset(reset),

            .dp_addr_i(dp_addr),
            .fifo0_load_i(fifo0_load),
            .fifo1_load_i(fifo1_load),
            .fifo0_en_load_i(fifo0_en_load),
            .clear_add_i(clear_add),
            .latch_add_i(latch_add),
            .data_en_i(data_en),
            .data_zero_i(data_zero),
            .latch_shift_i(latch_shift),
	    .last_bit_i(last_bit),
            .one_bit_i(one_bit),
	    .zero_bit_i(zero_bit)
        );
    end endgenerate

    cube_control cube_control (
        .clk(clk),
        .reset(reset),
	.drq(drq),
	.finished(finished),

	.dp_addr(dp_addr),
        .fifo0_load(fifo0_load),
        .fifo1_load(fifo1_load),
        .fifo0_en_load(fifo0_en_load),
        .clear_add(clear_add),
        .latch_add(latch_add),
        .data_en(data_en),
        .data_zero(data_zero),
        .latch_shift(latch_shift),
	.last_bit_out(last_bit),
        .one_bit(one_bit),
	.zero_bit(zero_bit)
    );

endmodule
