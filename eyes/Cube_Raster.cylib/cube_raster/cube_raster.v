`include "cypress.v"

module cube_control (
	input  wire clk,
	input  wire reset,
	output reg  drq,
	output reg  drq_d,
	output reg  finished,

	output reg  [2:0] dp_addr,
	output reg  fifo0_load,
	output reg  fifo1_load,
	output wire fifo0_en_load,
	output reg  clear_add,
	output reg  latch_add,
	output reg  data_en,
	output reg  data_zero,
	output reg  latch_shift,
	output reg  last_bit_out,
	output wire one_bit,
	output wire zero_bit);

    reg [1:0] dma_count_ctrl;
    reg [1:0] offset_ctrl;
    reg [1:0] byte_count, byte_count_i;
    reg [2:0] bit_count, bit_count_i;
    reg [4:0] state, state_i;
    reg last_bit, last_bit_i;
    reg offset_state, offset_load;
    reg fifo0_cpu;
    reg data_zero_latch;
    reg dma_req;
    reg data_stop, data_start;
    reg dma_tc;
    reg finished_i;
    
    wire offset_fifo_empty;
    wire zero_cmp;
    wire one_cmp;
    wire loop_load_inh;
    wire [1:0] pwm_ctrl;
    wire start;
    wire loop_load;
    wire dma_continue;
    wire pwm_tc;
    wire offset_tc;
    wire dma_tc_i;
    wire enable;
    wire ack_finished;
    wire drq_i;

    always @(posedge clk) begin : sync_output_proc
	finished    <= finished_i;
	drq	    <= drq_i;
	drq_d	    <= drq;
    end /* sync_output_proc */

    assign zero_bit	= !zero_cmp;
    assign one_bit	= !one_cmp;
    assign start	= !offset_fifo_empty;
    assign loop_load	= !loop_load_inh | start;

    assign fifo0_en_load    = fifo0_cpu | dma_continue;
    assign dma_continue	    = offset_tc & !loop_load & data_en & !dma_tc;
    assign drq_i	    = dma_req | dma_continue;

    localparam
	DEC_DEC		    = 0,
	DEC_LOAD_F0	    = 1,
	DEC_INH		    = 2,
	DEC_LOAD_D0	    = 3;

    assign pwm_ctrl	    = pwm_tc ? DEC_LOAD_F0 : DEC_DEC;

    wire [7:0] control; 
    
    cy_psoc3_control #(.cy_force_order(`TRUE)) ctrl (
	.control(control)
    );
    
    assign enable	= control[0];
    assign ack_finished	= control[1];

    localparam
    	DP_NOP	            = 0,
    	DP_FETCH_RGB	    = 1, /* Clear A0 for low path */
    	DP_FETCH_DIM	    = 2,
    	DP_FETCH_TMP	    = 3,
    	DP_ADD		    = 4,
   	DP_SHIFT_OP_L	    = 5,
    	DP_SHIFT_OP_R	    = 6,
    	DP_SHIFT_OUT	    = 7;
    
    localparam
	STATE_IDLE	    = 0,
	STATE_DMA	    = 1,
	STATE_MULT_INIT_0   = 2,
	STATE_MULT_INIT_1   = 3,
	STATE_MULT_INIT_2   = 4,
	STATE_MULT_INIT_3   = 5,
	STATE_MULT_INIT_4   = 6,
	STATE_MULT_INIT_5   = 7,
	STATE_MULT_INIT_6   = 8,
	STATE_MULT_INIT_7   = 9,
	STATE_MULT_INIT_8   = 10,
	STATE_MULT_INIT_9   = 11,
	STATE_MULT_INIT_10  = 12,
	STATE_MULT_INIT_11  = 13,
        STATE_MULT_INIT_12  = 14,
        STATE_MULT_INIT_13  = 15,
	STATE_MULT_LOOP_0   = 16,
	STATE_MULT_LOOP_1   = 17,
	STATE_MULT_LOOP_2   = 18,
	STATE_MULT_LOOP_3   = 19,
	STATE_MULT_LOOP_4   = 20,
	STATE_DATA_INIT	    = 21,
	STATE_DATA_SHIFT    = 22,
	STATE_DATA_WAIT	    = 23,
	STATE_FINISHED	    = 24;

    always @(state or enable or start or bit_count or byte_count or pwm_tc or 
		last_bit or offset_tc or ack_finished) begin : next_state_proc
	state_i <= state;
	bit_count_i <= bit_count;
	byte_count_i <= byte_count;
	last_bit_i <= last_bit;

	case (state)
	    STATE_IDLE: begin
		if (enable)
		    state_i <= STATE_DMA;
	    end

	    STATE_DMA: begin
		if (start)
		    state_i <= STATE_MULT_INIT_0;

		byte_count_i <= 3;
	    end

	    STATE_MULT_INIT_0: state_i <= STATE_MULT_INIT_1;
	    STATE_MULT_INIT_1: state_i <= STATE_MULT_INIT_2;
	    STATE_MULT_INIT_2: state_i <= STATE_MULT_INIT_3;
	    STATE_MULT_INIT_3: state_i <= STATE_MULT_INIT_4;
	    STATE_MULT_INIT_4: state_i <= STATE_MULT_INIT_5;
	    STATE_MULT_INIT_5: state_i <= STATE_MULT_INIT_6;
	    STATE_MULT_INIT_6: state_i <= STATE_MULT_INIT_7;
	    STATE_MULT_INIT_7: state_i <= STATE_MULT_INIT_8;
	    STATE_MULT_INIT_8: state_i <= STATE_MULT_INIT_9;
	    STATE_MULT_INIT_9: state_i <= STATE_MULT_INIT_10;
	    STATE_MULT_INIT_10: state_i <= STATE_MULT_INIT_11;
	    STATE_MULT_INIT_11: state_i <= STATE_MULT_INIT_12;
	    STATE_MULT_INIT_12: state_i <= STATE_MULT_INIT_13;
	    STATE_MULT_INIT_13: begin
		state_i <= STATE_MULT_LOOP_1;
		bit_count_i <= 1;
		byte_count_i <= byte_count + 1;
	    end
	    STATE_MULT_LOOP_0: state_i <= STATE_MULT_LOOP_1;
	    STATE_MULT_LOOP_1: state_i <= STATE_MULT_LOOP_2;
	    STATE_MULT_LOOP_2: state_i <= STATE_MULT_LOOP_3;
	    STATE_MULT_LOOP_3: state_i <= STATE_MULT_LOOP_4;
	    STATE_MULT_LOOP_4: begin
		bit_count_i <= bit_count + 1;

		if (!bit_count)
		    state_i <= STATE_DATA_INIT;
		else
		    state_i <= STATE_MULT_LOOP_0;
	    end

	    STATE_DATA_INIT: begin
		bit_count_i <= 1;
		last_bit_i <= 0;
		if (pwm_tc)
		    state_i <= STATE_DATA_SHIFT;
		if (byte_count == 3)
		    byte_count_i <= 0;
	    end

	    STATE_DATA_SHIFT: begin
		if ((bit_count == 7) && !last_bit) begin
		    last_bit_i <= 1;
		    state_i <= STATE_DATA_SHIFT;
		end else if (last_bit)
		    if ((byte_count == 2) && !start && offset_tc) 
			state_i <= STATE_FINISHED;
		    else begin
			state_i <= STATE_MULT_INIT_0;
		    end
		else
		    state_i <= STATE_DATA_WAIT;
	    end

	    STATE_DATA_WAIT: begin
		if (pwm_tc) begin
		    state_i <= STATE_DATA_SHIFT;
		    bit_count_i <= bit_count + 1;
		end
	    end

	    STATE_FINISHED: begin
		if (ack_finished)
		    state_i <= STATE_IDLE;
		if (pwm_tc)
		    last_bit_i <= 0;
	    end

	endcase
    end /* next_state_proc */

    always @(posedge clk) begin : sync_proc
	if (reset) begin
	    state <= STATE_IDLE;
	    bit_count <= 0;
	    byte_count <= 0;
	    last_bit <= 0;
	end else begin
	    state <= state_i;
	    bit_count <= bit_count_i;
	    byte_count <= byte_count_i;
	    last_bit <= last_bit_i;
	end
    end /* sync_proc */

    always @(state or pwm_tc or offset_tc or start or 
		offset_state or last_bit or bit_count or 
		byte_count or dma_continue or loop_load) begin : control_proc
	dp_addr		<= DP_SHIFT_OUT;
	offset_ctrl	<= DEC_INH; 
	dma_count_ctrl	<= DEC_INH;
	fifo0_load	<= 0;
	fifo0_cpu	<= 0;
	fifo1_load	<= 0;
	latch_add	<= 0;
	clear_add	<= 0;
	latch_shift	<= 0;
	data_stop	<= 0;
	data_start	<= 0;
	dma_req		<= 0;
	data_zero_latch	<= 0;
	finished_i	<= 0;
	offset_load	<= 0;

	case (state)
	    STATE_IDLE:
		fifo0_cpu <= 1;

	    STATE_DMA: begin
		fifo0_cpu <= 1;
		dma_req <= 1;
		dma_count_ctrl <= DEC_LOAD_D0;
	    end

	    /* Multiplier initialisation */
	    STATE_MULT_INIT_0:
		dp_addr <= DP_FETCH_DIM;

	    STATE_MULT_INIT_1: begin
		dp_addr <= DP_FETCH_DIM;
		fifo1_load <= 1;
		if (offset_tc & start)
		    if (offset_state)
			offset_ctrl <= DEC_LOAD_D0;
		    else
			offset_ctrl <= DEC_LOAD_F0;
	    end

	    STATE_MULT_INIT_2: begin
		dp_addr <= DP_FETCH_RGB;
		if (offset_state & offset_tc)
		    offset_ctrl <= DEC_LOAD_F0;
	    end

	    STATE_MULT_INIT_3: begin
		dp_addr <= DP_NOP;
		fifo1_load <= 1;
		latch_add <= 1;
		if (loop_load & !(offset_state & offset_tc))
		    fifo0_load <= 1;
		if (last_bit && (byte_count == 1))
		    offset_ctrl <= DEC_DEC;
		if (offset_state)
		    offset_load <= 1;
	    end

	    STATE_MULT_INIT_4: begin
                dp_addr <= DP_FETCH_DIM;
		if (dma_continue)
		    dma_count_ctrl <= DEC_DEC;
	    end

            STATE_MULT_INIT_13:
                dp_addr <= DP_FETCH_TMP;

	    /* Multiplier iteration */
	    STATE_MULT_LOOP_0: begin
		dp_addr <= DP_FETCH_TMP;
		latch_add <= 1;
	    end

	    STATE_MULT_LOOP_1: begin
		dp_addr <= DP_SHIFT_OP_R;
		fifo1_load <= 1;
		clear_add <= 1;
	    end

	    STATE_MULT_LOOP_2:
		dp_addr <= DP_ADD;

	    STATE_MULT_LOOP_3:
		dp_addr <= DP_FETCH_TMP;

	    STATE_MULT_LOOP_4: begin
		dp_addr <= DP_SHIFT_OP_L;
		fifo1_load <= 1;
	    end

	    STATE_DATA_INIT: begin
		dp_addr <= DP_FETCH_TMP;
		if (byte_count == 0)
		    data_zero_latch <= 1;
	    end

	    STATE_DATA_SHIFT: begin
		latch_shift <= 1;
		dp_addr <= DP_SHIFT_OUT;
		if (bit_count == 1)
		    data_start <= 1;
	    end

	    STATE_DATA_WAIT:
		dp_addr <= DP_FETCH_TMP;

	    STATE_FINISHED: begin
		finished_i <= 1;
		if (pwm_tc & !last_bit)
		    data_stop <= 1;
	    end

	endcase
    end /* control_proc */

    always @(posedge clk) begin : data_zero_proc
	if (reset)
	    data_zero <= 0;
	else if (pwm_tc & data_zero_latch)
	    data_zero <= start & !offset_tc;
    end /* data_zero_proc */

    always @(posedge clk) begin : data_en_proc
        if (reset | data_stop)
            data_en <= 0;
        else if (data_start)
            data_en <= 1;
    end /* data_en_proc */

    always @(posedge clk) begin : last_bit_proc
	if (pwm_tc)
	    last_bit_out <= 0;
	else if (last_bit & latch_shift)
	    last_bit_out <= 1;
    end /* last_bit_proc */

    always @(posedge clk) begin : dma_tc_proc
	if (reset)
	    dma_tc <= 0;
	else if (!dma_continue)
	    dma_tc <= dma_tc_i;
    end /* dma_tc_proc */

    always @(posedge clk) begin : offset_state_proc
	if (reset | offset_load)
	    offset_state <= 0;
	else if (fifo0_en_load)
	    offset_state <= 1;
    end /* offset_state_proc */

    parameter dp_dec_config = {
        `CS_ALU_OP__DEC, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM0: Decrement*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM1: Load F0*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM2: Inhibit*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC___D0, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM3: Load D0*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM4: */
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM5: */
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM6: */
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM7: */ 
         8'hFF, 8'h00, /*CFG9: */
         8'hFF, 8'hFF, /*CFG11-10: */
        `SC_CMPB_A0_D1, `SC_CMPA_A0_D1, `SC_CI_B_ARITH,
        `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
        `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
        `SC_SI_A_DEFSI, /*CFG13-12: */
        `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
         1'h0, `SC_FIFO1_BUS, `SC_FIFO0_BUS,
        `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
        `SC_FB_NOCHN, `SC_CMP1_NOCHN,
        `SC_CMP0_NOCHN, /*CFG15-14: */
         10'h00, `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
        `SC_FIFO_LEVEL,`SC_FIFO_ASYNC,`SC_EXTCRC_DSBL,
        `SC_WRK16CAT_DSBL /*CFG17-16: */
    };

    parameter dp_dec_config_high = {
        `CS_ALU_OP__DEC, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM0: Decrement*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM1: Load F0*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM2: Inhibit*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC___D0, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM3: Load D0*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM4: */
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM5: */
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM6: */
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM7: */ 
         8'hFF, 8'h00, /*CFG9: */
         8'hFF, 8'hFF, /*CFG11-10: */
        `SC_CMPB_A0_D1, `SC_CMPA_A0_D1, `SC_CI_B_ARITH,
        `SC_CI_A_CHAIN, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
        `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
        `SC_SI_A_DEFSI, /*CFG13-12: */
        `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
         1'h0, `SC_FIFO1_BUS, `SC_FIFO0_BUS,
        `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
        `SC_FB_NOCHN, `SC_CMP1_NOCHN,
        `SC_CMP0_CHNED, /*CFG15-14: */
         10'h00, `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
        `SC_FIFO_LEVEL,`SC_FIFO_ASYNC,`SC_EXTCRC_DSBL,
        `SC_WRK16CAT_DSBL /*CFG17-16: */
    };

    cy_psoc3_dp8 #(.cy_dpconfig_a(dp_dec_config)) pwm8 (
        /*  input                   */  .reset(reset),
        /*  input                   */  .clk(clk),
        /*  input	            */  .cs_addr({1'b0, pwm_ctrl}),
        /*  input                   */  .route_si(1'b0),
        /*  input                   */  .route_ci(1'b0),
        /*  input                   */  .f0_load(1'b0),
        /*  input                   */  .f1_load(1'b0),
        /*  input                   */  .d0_load(1'b0),
        /*  input                   */  .d1_load(1'b0),
        /*  output                  */  .ce0(),
        /*  output                  */  .cl0(zero_cmp),
        /*  output                  */  .z0(pwm_tc),
        /*  output                  */  .ff0(),
        /*  output                  */  .ce1(),
        /*  output                  */  .cl1(one_cmp),
        /*  output                  */  .z1(),
        /*  output                  */  .ff1(),
        /*  output                  */  .ov_msb(),
        /*  output                  */  .co_msb(),
        /*  output                  */  .cmsb(),
        /*  output                  */  .so(),
        /*  output                  */  .f0_bus_stat(),
        /*  output                  */  .f0_blk_stat(),
        /*  output                  */  .f1_bus_stat(),
        /*  output                  */  .f1_blk_stat()
    );

    cy_psoc3_dp8 #(.cy_dpconfig_a(dp_dec_config)) offset_count (
        /*  input                   */  .reset(reset),
        /*  input                   */  .clk(clk),
        /*  input	            */  .cs_addr({1'b0, offset_ctrl}),
        /*  input                   */  .route_si(1'b0),
        /*  input                   */  .route_ci(1'b0),
        /*  input                   */  .f0_load(1'b0),
        /*  input                   */  .f1_load(1'b0),
        /*  input                   */  .d0_load(1'b0),
        /*  input                   */  .d1_load(1'b0),
        /*  output                  */  .ce0(),
        /*  output                  */  .cl0(),
        /*  output                  */  .z0(offset_tc),
        /*  output                  */  .ff0(),
        /*  output                  */  .ce1(),
        /*  output                  */  .cl1(loop_load_inh),
        /*  output                  */  .z1(),
        /*  output                  */  .ff1(),
        /*  output                  */  .ov_msb(),
        /*  output                  */  .co_msb(),
        /*  output                  */  .cmsb(),
        /*  output                  */  .so(),
        /*  output                  */  .f0_bus_stat(),
        /*  output                  */  .f0_blk_stat(offset_fifo_empty),
        /*  output                  */  .f1_bus_stat(),
        /*  output                  */  .f1_blk_stat()
    );

    cy_psoc3_dp16 #(.cy_dpconfig_a(dp_dec_config),
		    .cy_dpconfig_b(dp_dec_config_high)) dma_count (
        /*  input                   */  .reset(reset),
        /*  input                   */  .clk(clk),
        /*  input	            */  .cs_addr({1'b0, dma_count_ctrl}),
        /*  input                   */  .route_si(1'b0),
        /*  input                   */  .route_ci(1'b0),
        /*  input                   */  .f0_load(1'b0),
        /*  input                   */  .f1_load(1'b0),
        /*  input                   */  .d0_load(1'b0),
        /*  input                   */  .d1_load(1'b0),
        /*  output                  */  .ce0(),
        /*  output                  */  .cl0(),
        /*  output                  */  .z0({dma_tc_i, nc_0}),
        /*  output                  */  .ff0(),
        /*  output                  */  .ce1(),
        /*  output                  */  .cl1(),
        /*  output                  */  .z1(),
        /*  output                  */  .ff1(),
        /*  output                  */  .ov_msb(),
        /*  output                  */  .co_msb(),
        /*  output                  */  .cmsb(),
        /*  output                  */  .so(),
        /*  output                  */  .f0_bus_stat(),
        /*  output                  */  .f0_blk_stat(),
        /*  output                  */  .f1_bus_stat(),
        /*  output                  */  .f1_blk_stat()
    );

endmodule

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

`define NUM_SHIFTERS 10

module cube_raster (
	output wire drq,
	output wire drq_d,
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
	.drq_d(drq_d),
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
