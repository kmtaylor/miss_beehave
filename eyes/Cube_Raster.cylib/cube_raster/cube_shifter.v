`include "cypress.v"

module cube_shifter (
	output reg  sout,
	input  wire clk,
	input  wire reset,

	input  wire [2:0] dp_addr_i,
	input  wire fifo0_load_i,
	input  wire fifo1_load_i,
	input  wire fifo0_en_load_i,
	input  wire clear_add_i,
	input  wire latch_add_i,
	input  wire data_en_i,
	input  wire data_zero_i,
	input  wire latch_shift_i,
	input  wire last_bit_i,
	input  wire one_bit_i,
	input  wire zero_bit_i);

    wire shift_out_i;
    wire do_add_i;
    wire data_out;
    wire shift_data;
    wire sout_i;
    
    reg [1:0] shift_out;
    reg do_add;
    reg [2:0] dp_addr;
    reg dshifter_fifo0_load;
    reg dshifter_fifo1_load;
    reg dshifter_fifo0_en_load;
    reg dshifter_clear_add;
    reg dshifter_latch_add;
    reg dshifter_data_en;
    reg dshifter_data_zero;
    reg dshifter_latch_shift;
    reg dshifter_last_bit;
    reg dshifter_one_bit;
    reg dshifter_zero_bit;

    assign shift_data = dshifter_last_bit ? shift_out[1] : shift_out[0];
    assign data_out = dshifter_data_zero ? dshifter_zero_bit : 
                            shift_data ? dshifter_one_bit : dshifter_zero_bit;
    assign sout_i   = dshifter_data_en ? data_out : 0;

    localparam DP_NOP = 0;

    always @(posedge clk) begin : sync_input_proc
	dshifter_fifo0_load	<= fifo0_load_i;
	dshifter_fifo1_load	<= fifo1_load_i;
	dshifter_fifo0_en_load	<= fifo0_en_load_i;
	dshifter_clear_add	<= clear_add_i;
	dshifter_latch_add	<= latch_add_i;
	dshifter_data_en	<= data_en_i;
	dshifter_data_zero	<= data_zero_i;
	dshifter_latch_shift	<= latch_shift_i;
	dshifter_last_bit	<= last_bit_i;
	dshifter_one_bit	<= one_bit_i;
	dshifter_zero_bit	<= zero_bit_i;
	sout			<= sout_i;
    end /* sync_input_proc */

    always @(posedge clk) begin : sync_addr_proc
	if (!do_add & dshifter_clear_add) begin
	    dp_addr <= DP_NOP;
	end else begin
	    dp_addr <= dp_addr_i;
	end
    end /* sync_addr_proc */

    always @(posedge clk) begin : shift_out_proc
	if (reset)
	    shift_out <= 0;
	else if (dshifter_latch_shift) begin
	    shift_out[0] <= shift_out_i;
	    shift_out[1] <= shift_out[0];
	end
    end /* shift_out_proc */

    always @(posedge clk) begin : do_add_proc
        if (reset | dshifter_clear_add)
            do_add <= 0;
        else if (dshifter_latch_add)
            if (do_add_i) do_add <= 1;
    end /* do_add_proc */

    parameter dp_shift_config_low = {
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM0: NOP*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM1: Fetch RGB*/
        `CS_ALU_OP__XOR, `CS_SRCA_A0, `CS_SRCB_A0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM2: Fetch Dim*/
        `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM3: Fetch Tmp*/
        `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
        `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM4: Add*/
        `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
        `CS_SHFT_OP___SL, `CS_A0_SRC_NONE, `CS_A1_SRC__ALU,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM5: Shift op left*/
        `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
        `CS_SHFT_OP___SR, `CS_A0_SRC_NONE, `CS_A1_SRC__ALU,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM6: Shift op right*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM7: Shift out*/
         8'hFF, 8'hFF,  /*CFG9: */
         8'hFF, 8'hFF,  /*CFG11-10: */
        `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
        `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
        `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
        `SC_SI_A_CHAIN, /*CFG13-12: */
        `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
         1'h0, `SC_FIFO1_ALU, `SC_FIFO0_ALU,
        `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
        `SC_FB_NOCHN, `SC_CMP1_NOCHN,
        `SC_CMP0_NOCHN, /*CFG15-14: */
         6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_ON,
         2'b00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
        `SC_FIFO_LEVEL /*CFG17-16: */
    ,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL};

    parameter dp_shift_config_high = {
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM0: NOP*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM1: Fetch RGB*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC___D0, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM2: Fetch Dim*/
        `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM3: Fetch Tmp*/
        `CS_ALU_OP__ADD, `CS_SRCA_A0, `CS_SRCB_A1,
        `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM4: Add*/
        `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
        `CS_SHFT_OP___SL, `CS_A0_SRC_NONE, `CS_A1_SRC__ALU,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM5: Shift op left*/
        `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
        `CS_SHFT_OP___SR, `CS_A0_SRC_NONE, `CS_A1_SRC__ALU,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM6: Shift op right*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM7: Shift out*/
         8'hFF, 8'hFF,  /*CFG9: */
         8'hFF, 8'hFF,  /*CFG11-10: */
        `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
        `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
        `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
        `SC_SI_A_CHAIN, /*CFG13-12: */
        `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
         1'h0, `SC_FIFO1_ALU, `SC_FIFO0_ALU,
        `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
        `SC_FB_NOCHN, `SC_CMP1_NOCHN,
        `SC_CMP0_NOCHN, /*CFG15-14: */
         6'h00, `SC_FIFO1_DYN_ON,`SC_FIFO0_DYN_ON,
         2'b00,`SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
        `SC_FIFO_LEVEL, /*CFG17-16: */
	`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,`SC_WRK16CAT_DSBL};

    cy_psoc3_dp16 #(.cy_dpconfig_a(dp_shift_config_low),
                    .cy_dpconfig_b(dp_shift_config_high)) dshifter (
        .reset(reset),
        .clk(clk),
        .cs_addr(dp_addr),
        .route_si(1'b0),
        .route_ci(1'b0),
        .f0_load(dshifter_fifo0_load),
        .f1_load(dshifter_fifo1_load),
        .d0_load(dshifter_fifo0_en_load),
        .d1_load(1'b0),

        /* Outputs */
        .cmsb({nc_0, do_add_i}),
        .so({shift_out_i, nc_1})
    );

endmodule
