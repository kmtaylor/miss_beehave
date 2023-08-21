#define `$INSTANCE_NAME`_NUM_OUTPUTS 10

/* Data DMA Addresses */
#define `$INSTANCE_NAME`_RGB_DATA_0 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_0__cube_shifter_dshifter_u0__F0_REG
#define `$INSTANCE_NAME`_RGB_DATA_1 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_1__cube_shifter_dshifter_u0__F0_REG
#define `$INSTANCE_NAME`_RGB_DATA_2 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_2__cube_shifter_dshifter_u0__F0_REG
#define `$INSTANCE_NAME`_RGB_DATA_3 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_3__cube_shifter_dshifter_u0__F0_REG
#define `$INSTANCE_NAME`_RGB_DATA_4 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_4__cube_shifter_dshifter_u0__F0_REG
#define `$INSTANCE_NAME`_RGB_DATA_5 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_5__cube_shifter_dshifter_u0__F0_REG
#define `$INSTANCE_NAME`_RGB_DATA_6 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_6__cube_shifter_dshifter_u0__F0_REG
#define `$INSTANCE_NAME`_RGB_DATA_7 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_7__cube_shifter_dshifter_u0__F0_REG
#define `$INSTANCE_NAME`_RGB_DATA_8 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_8__cube_shifter_dshifter_u0__F0_REG
#define `$INSTANCE_NAME`_RGB_DATA_9 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_9__cube_shifter_dshifter_u0__F0_REG

#define `$INSTANCE_NAME`_DIM_VAL_0 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_0__cube_shifter_dshifter_u1__D0_REG
#define `$INSTANCE_NAME`_DIM_VAL_1 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_1__cube_shifter_dshifter_u1__D0_REG
#define `$INSTANCE_NAME`_DIM_VAL_2 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_2__cube_shifter_dshifter_u1__D0_REG
#define `$INSTANCE_NAME`_DIM_VAL_3 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_3__cube_shifter_dshifter_u1__D0_REG
#define `$INSTANCE_NAME`_DIM_VAL_4 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_4__cube_shifter_dshifter_u1__D0_REG
#define `$INSTANCE_NAME`_DIM_VAL_5 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_5__cube_shifter_dshifter_u1__D0_REG
#define `$INSTANCE_NAME`_DIM_VAL_6 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_6__cube_shifter_dshifter_u1__D0_REG
#define `$INSTANCE_NAME`_DIM_VAL_7 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_7__cube_shifter_dshifter_u1__D0_REG
#define `$INSTANCE_NAME`_DIM_VAL_8 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_8__cube_shifter_dshifter_u1__D0_REG
#define `$INSTANCE_NAME`_DIM_VAL_9 \
	    `$INSTANCE_NAME`_cube_raster_slave_shifter_9__cube_shifter_dshifter_u1__D0_REG

/* Offset/Count Registers */
#define `$INSTANCE_NAME`_FIFO_LOOP \
	    `$INSTANCE_NAME`_cube_raster_cube_control_offset_count_u0__D1_REG
/* Offset/Count DMA Addresses */
#define `$INSTANCE_NAME`_INACTIVE_COUNT \
	    `$INSTANCE_NAME`_cube_raster_cube_control_offset_count_u0__D0_REG
#define `$INSTANCE_NAME`_ACTIVE_COUNT \
	    `$INSTANCE_NAME`_cube_raster_cube_control_offset_count_u0__F0_REG

/* DMA Registers */
#define `$INSTANCE_NAME`_DMA_COUNT \
	    `$INSTANCE_NAME`_cube_raster_cube_control_dma_count_u0__16BIT_D0_REG

/* PWM Registers */
#define `$INSTANCE_NAME`_PWM_DISABLE_FIFO \
	    `$INSTANCE_NAME`_cube_raster_cube_control_pwm8_u0__DP_AUX_CTL_REG
#define `$INSTANCE_NAME`_PWM_PERIOD \
	    `$INSTANCE_NAME`_cube_raster_cube_control_pwm8_u0__F0_REG
#define `$INSTANCE_NAME`_PWM_COMP0 \
	    `$INSTANCE_NAME`_cube_raster_cube_control_pwm8_u0__D0_REG
#define `$INSTANCE_NAME`_PWM_COMP1 \
	    `$INSTANCE_NAME`_cube_raster_cube_control_pwm8_u0__D1_REG

/* Control Register */
#define `$INSTANCE_NAME`_CONTROL \
	    `$INSTANCE_NAME`_cube_raster_cube_control_ctrl__CONTROL_REG

#define `$INSTANCE_NAME`_ENABLE	    (1 << 0)
#define `$INSTANCE_NAME`_RESTART    (1 << 1)

extern void `$INSTANCE_NAME`_SetupDMA(uint16 chain_length, uint8 **rgb,
	uint8 *offset, uint8 *count);
extern void `$INSTANCE_NAME`_SetupIRQ(uint16 chain_length, uint8 **rgb,
	uint8 *offset, uint8 *count);

extern void `$INSTANCE_NAME`_UpdateDMA(void);
extern void `$INSTANCE_NAME`_UpdateIRQ(void);

extern void `$INSTANCE_NAME`_SetBrightness(uint8 level);
