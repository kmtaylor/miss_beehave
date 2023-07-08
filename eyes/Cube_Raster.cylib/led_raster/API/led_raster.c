#include <project.h>

#define OFFSET_SIZE 1
#define COUNT_SIZE  1
#define RGB_SIZE    3
#define NUM_TDS	    (`$INSTANCE_NAME`_NUM_OUTPUTS + OFFSET_SIZE + COUNT_SIZE)

#define REQ_PER_BURST   1
#define PRESERVE_TDS    1
#define DATA_MEM_LOC	HI16(CYDEV_SRAM_BASE)
#define UDB_REG_MEM_LOC	HI16(CYDEV_PERIPH_BASE)

#define REG_WRITE(_reg, _val) ((*(reg8 *) _reg) = _val)

static uint8 td[NUM_TDS];
static uint8 dma_ch[NUM_TDS];
static uint8 **rgb_d;
static uint8 *offset_d;
static uint8 *count_d;
static volatile uint8 rgb_i, offset_i, count_i;

void `$INSTANCE_NAME`_UpdateIRQ(void) {
    rgb_i = 0;
    offset_i = 0;
    count_i = 0;

    REG_WRITE(`$INSTANCE_NAME`_CONTROL, `$INSTANCE_NAME`_RESTART);
    REG_WRITE(`$INSTANCE_NAME`_CONTROL, `$INSTANCE_NAME`_ENABLE);
}

void `$INSTANCE_NAME`_UpdateDMA(void) {
    uint8 i;

    for (i = 0; i < NUM_TDS; i++) {
        CyDmaChDisable(dma_ch[i]);
        CyDmaChSetInitialTd(dma_ch[i], td[i]);
        CyDmaChEnable(dma_ch[i], PRESERVE_TDS);
    }

    REG_WRITE(`$INSTANCE_NAME`_CONTROL, `$INSTANCE_NAME`_RESTART);
    REG_WRITE(`$INSTANCE_NAME`_CONTROL, `$INSTANCE_NAME`_ENABLE);
}

void `$INSTANCE_NAME`_SetBrightness(uint8 level) {
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_0, level);
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_1, level);
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_2, level);
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_3, level);
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_4, level);
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_5, level);
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_6, level);
}

CY_ISR(`$INSTANCE_NAME`_isr) {
    uint8 i;

    for (i = 0; i < RGB_SIZE; i++) { 
        REG_WRITE(LED_RASTER_RGB_DATA_0, rgb_d[0][rgb_i + i]);
        REG_WRITE(LED_RASTER_RGB_DATA_1, rgb_d[1][rgb_i + i]);
        REG_WRITE(LED_RASTER_RGB_DATA_2, rgb_d[2][rgb_i + i]);
        REG_WRITE(LED_RASTER_RGB_DATA_3, rgb_d[3][rgb_i + i]);
        REG_WRITE(LED_RASTER_RGB_DATA_4, rgb_d[4][rgb_i + i]);
        REG_WRITE(LED_RASTER_RGB_DATA_5, rgb_d[5][rgb_i + i]);
        REG_WRITE(LED_RASTER_RGB_DATA_6, rgb_d[6][rgb_i + i]);
    }
    rgb_i += RGB_SIZE;
    
    REG_WRITE(LED_RASTER_INACTIVE_COUNT, offset_d[offset_i++]);
    REG_WRITE(LED_RASTER_ACTIVE_COUNT, count_d[count_i++]);
}

static void write_udb_constants(uint8 chain_length) {
#define PERIOD_800KHZ 30
    REG_WRITE(`$INSTANCE_NAME`_PWM_DISABLE_FIFO, 0x03);
    REG_WRITE(`$INSTANCE_NAME`_FIFO_LOOP, 0x02);
    REG_WRITE(`$INSTANCE_NAME`_PWM_PERIOD, PERIOD_800KHZ);
    REG_WRITE(`$INSTANCE_NAME`_PWM_COMP0, (PERIOD_800KHZ * 20) / 25);
    REG_WRITE(`$INSTANCE_NAME`_PWM_COMP1, (PERIOD_800KHZ * 12) / 25);
    REG_WRITE(`$INSTANCE_NAME`_DMA_COUNT, chain_length - 1);
    `$INSTANCE_NAME`_SetBrightness(0xFF);
}

void `$INSTANCE_NAME`_SetupIRQ(uint8 chain_length, uint8 **rgb, uint8 *offset,
                uint8 *count) {
    rgb_d = rgb;
    offset_d = offset;
    count_d = count;

    write_udb_constants(chain_length);

    `$INSTANCE_NAME`_DRQ_StartEx(`$INSTANCE_NAME`_isr);

    `$INSTANCE_NAME`_UpdateIRQ();
}

void `$INSTANCE_NAME`_SetupDMA(uint8 chain_length, uint8 **rgb, uint8 *offset,
		uint8 *count) {
    rgb_d = rgb;
    offset_d = offset;
    count_d = count;

    write_udb_constants(chain_length);

#define DMA_CH_INIT(_ch, _name, _size, _prio) \
        dma_ch[_ch] = LED_RASTER_DMA_##_name##_DmaInitialize(_size,         \
            REQ_PER_BURST, DATA_MEM_LOC, UDB_REG_MEM_LOC);                  \
        CyDmaChPriority(dma_ch[_ch], _prio);

    DMA_CH_INIT(0, 0, RGB_SIZE, 0);
    DMA_CH_INIT(1, 1, RGB_SIZE, 0);
    DMA_CH_INIT(2, 2, RGB_SIZE, 0);
    DMA_CH_INIT(3, 3, RGB_SIZE, 0);
    DMA_CH_INIT(4, 4, RGB_SIZE, 0);
    DMA_CH_INIT(5, 5, RGB_SIZE, 0);
    DMA_CH_INIT(6, 6, RGB_SIZE, 0);
    DMA_CH_INIT(7, O, OFFSET_SIZE, 0);
    DMA_CH_INIT(8, C, COUNT_SIZE, 7); /* Must be lower priority */

#define DMA_TD_INIT(_i, _size, _data, _reg) \
        td[_i] = CyDmaTdAllocate();                                         \
        CyDmaTdSetConfiguration(td[_i], _size,                              \
            DMA_DISABLE_TD, CY_DMA_TD_INC_SRC_ADR);                         \
        CyDmaTdSetAddress(td[_i], LO16((uint32) _data), LO16((uint32) _reg));

    DMA_TD_INIT(0, RGB_SIZE * chain_length, rgb_d[0], LED_RASTER_RGB_DATA_0);
    DMA_TD_INIT(1, RGB_SIZE * chain_length, rgb_d[1], LED_RASTER_RGB_DATA_1);
    DMA_TD_INIT(2, RGB_SIZE * chain_length, rgb_d[2], LED_RASTER_RGB_DATA_2);
    DMA_TD_INIT(3, RGB_SIZE * chain_length, rgb_d[3], LED_RASTER_RGB_DATA_3);
    DMA_TD_INIT(4, RGB_SIZE * chain_length, rgb_d[4], LED_RASTER_RGB_DATA_4);
    DMA_TD_INIT(5, RGB_SIZE * chain_length, rgb_d[5], LED_RASTER_RGB_DATA_5);
    DMA_TD_INIT(6, RGB_SIZE * chain_length, rgb_d[6], LED_RASTER_RGB_DATA_6);
    DMA_TD_INIT(7, OFFSET_SIZE * chain_length,
            offset_d, LED_RASTER_INACTIVE_COUNT);
    DMA_TD_INIT(8, COUNT_SIZE  * chain_length,
            count_d,  LED_RASTER_ACTIVE_COUNT);

    `$INSTANCE_NAME`_UpdateDMA();
}
