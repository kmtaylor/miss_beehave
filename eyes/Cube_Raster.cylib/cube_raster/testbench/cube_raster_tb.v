`timescale 1ns / 1ns

`include "cube_raster_top.v"

module cube_raster_tb();

    localparam CLK_PERIOD = 20;

    initial begin
	$dumpfile(``WAVE_FILE);
	$dumpvars;
	clk = 0;
	#(40_000 * CLK_PERIOD) $finish;
    end

    reg clk, reset;

    wire [6:0] sout;

    cube_raster cube_raster (
	.drq(drq),
	.sout(sout),
	.finished(finished),
	.clk(clk),
	.reset(reset)
    );

    always #CLK_PERIOD clk <= !clk;
    always @(clk) cube_raster.cube_control.ctrl.cpu_clock <= clk;
    always @(clk) cube_raster.cube_control.dma_count.U0.cpu_clock <= clk;
    always @(clk) cube_raster.cube_control.dma_count.U1.cpu_clock <= clk;
    always @(clk) cube_raster.cube_control.pwm8.U0.cpu_clock <= clk;
    always @(clk) cube_raster.cube_control.offset_count.U0.cpu_clock <= clk;
    always @(clk) cube_raster.slave_shifter[0].
			cube_shifter.dshifter.U0.cpu_clock <= clk;
    always @(clk) cube_raster.slave_shifter[0].
			cube_shifter.dshifter.U1.cpu_clock <= clk;
    
    localparam DISABLE_FIFO_0	    = 8'h01;
    localparam DISABLE_FIFO_1	    = 8'h02;
    localparam FIFO_0_THRESH_HALF   = 8'h04;
    localparam FIFO_1_THRESH_HALF   = 8'h08;

    localparam PERIOD		= 8'd30;
    
    localparam ENABLE		= 8'h01;
    localparam RESTART		= 8'h02;

`define WRITE_RGB(r, g, b) \
    cube_raster.slave_shifter[0].cube_shifter.dshifter.U0.fifo0_write(r); \
    cube_raster.slave_shifter[0].cube_shifter.dshifter.U0.fifo0_write(g); \
    cube_raster.slave_shifter[0].cube_shifter.dshifter.U0.fifo0_write(b);

    initial begin
	reset = 1;

	/* Setup FIFOs as one byte buffers */
	cube_raster.cube_control.pwm8.U0.aux_ctrl = 
		DISABLE_FIFO_0 | DISABLE_FIFO_1;

	/* Used for looping the data fifo */
	cube_raster.cube_control.offset_count.U0.d1_write(2);

	/* Period is 30 for 800KHz or 60 for 400KHz at 24MHz */
	cube_raster.cube_control.pwm8.U0.fifo0_write(PERIOD);

	/* FIFO must be setup before disabling reset so that FIFO RAM is in
	 * a defined state */
	#100 @(posedge clk) reset = 0; #100 @(posedge clk);

	/* Compare registers */
	cube_raster.cube_control.pwm8.U0.d0_write((PERIOD * 20) / 25);
	cube_raster.cube_control.pwm8.U0.d1_write((PERIOD * 12) / 25);

	cube_raster.slave_shifter[0].cube_shifter.dshifter.U1.d0_write(8'h80);

	/* Number of DMA requests to make */
	cube_raster.cube_control.dma_count.U0.d0_write(3);
	cube_raster.cube_control.dma_count.U1.d0_write(0);

	/* Enable module */
	cube_raster.cube_control.ctrl.control_write(ENABLE);

`define DMA_WRITE(r, g, b, i, a) \
	@(posedge drq); \
	@(posedge clk); \
	@(posedge clk); \
	`WRITE_RGB(r, g, b) \
	cube_raster.cube_control.offset_count.U0.d0_write(i); /* Inactive */ \
	cube_raster.cube_control.offset_count.U0.fifo0_write(a); /* Active */

`ifdef DMA_WRITE
	`DMA_WRITE(8'h87, 8'h65, 8'h43, 0, 1)
	`DMA_WRITE(8'h87, 8'h65, 8'h43, 0, 1)
	`DMA_WRITE(8'h87, 8'h65, 8'h43, 0, 1)
	`DMA_WRITE(8'h87, 8'h65, 8'h43, 0, 1)
`endif

	@(posedge finished); #10_000; @(posedge clk)
	cube_raster.cube_control.ctrl.control_write(RESTART);

	cube_raster.cube_control.ctrl.control_write(ENABLE);
	`DMA_WRITE(8'h87, 8'h65, 8'h43, 0, 4)
    end
 
endmodule
