#include <project.h>

#define OFFSET_SIZE 1
#define COUNT_SIZE  1
#define RGB_SIZE    3
#define NUM_TDS     (`$INSTANCE_NAME`_NUM_OUTPUTS + OFFSET_SIZE + COUNT_SIZE)

#define REQ_PER_BURST   1
#define PRESERVE_TDS    1
#define DATA_MEM_LOC    HI16(CYDEV_SRAM_BASE)
#define UDB_REG_MEM_LOC HI16(CYDEV_PERIPH_BASE)

#define REG_WRITE(_reg, _val) ((*(reg8 *) _reg) = _val)
#define REG16_WRITE(_reg, _val) ((*(reg16 *) _reg) = _val)

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

#define DMA_TD_ADDRESS(_i, _data, _reg) \
        CyDmaTdSetAddress(td[_i], LO16((uint32) _data), LO16((uint32) _reg));

static uint32 `$INSTANCE_NAME`_RGB_DATA[] = {
    `$INSTANCE_NAME`_RGB_DATA_0,
    `$INSTANCE_NAME`_RGB_DATA_1,
    `$INSTANCE_NAME`_RGB_DATA_2,
    `$INSTANCE_NAME`_RGB_DATA_3,
    `$INSTANCE_NAME`_RGB_DATA_4,
    `$INSTANCE_NAME`_RGB_DATA_5,
    `$INSTANCE_NAME`_RGB_DATA_6,
    `$INSTANCE_NAME`_RGB_DATA_7,
    `$INSTANCE_NAME`_RGB_DATA_8,
    `$INSTANCE_NAME`_RGB_DATA_9,
};

void `$INSTANCE_NAME`_UpdateDMA(uint8 **rgb) {
    uint8 i;

    for (i = 0; i < `$INSTANCE_NAME`_NUM_OUTPUTS; i++) {
        CyDmaChDisable(dma_ch[i]);
        DMA_TD_ADDRESS(i, rgb[i], `$INSTANCE_NAME`_RGB_DATA[i]);
        CyDmaChSetInitialTd(dma_ch[i], td[i]);
        CyDmaClearPendingDrq(dma_ch[i]);
        CyDmaChEnable(dma_ch[i], PRESERVE_TDS);
    }

    for (i = `$INSTANCE_NAME`_NUM_OUTPUTS; i < NUM_TDS; i++) {
        CyDmaChDisable(dma_ch[i]);
        CyDmaChSetInitialTd(dma_ch[i], td[i]);
        CyDmaClearPendingDrq(dma_ch[i]);
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
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_7, level);
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_8, level);
    REG_WRITE(`$INSTANCE_NAME`_DIM_VAL_9, level);
}

CY_ISR(`$INSTANCE_NAME`_isr) {
    uint8 i;

    for (i = 0; i < RGB_SIZE; i++) { 
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_0, rgb_d[0][rgb_i + i]);
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_1, rgb_d[1][rgb_i + i]);
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_2, rgb_d[2][rgb_i + i]);
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_3, rgb_d[3][rgb_i + i]);
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_4, rgb_d[4][rgb_i + i]);
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_5, rgb_d[5][rgb_i + i]);
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_6, rgb_d[6][rgb_i + i]);
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_7, rgb_d[7][rgb_i + i]);
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_8, rgb_d[8][rgb_i + i]);
        REG_WRITE(`$INSTANCE_NAME`_RGB_DATA_9, rgb_d[9][rgb_i + i]);
    }
    rgb_i += RGB_SIZE;
    
    REG_WRITE(`$INSTANCE_NAME`_INACTIVE_COUNT, offset_d[offset_i++]);
    REG_WRITE(`$INSTANCE_NAME`_ACTIVE_COUNT, count_d[count_i++]);
}

static void write_udb_constants(uint16 chain_length) {
#define PERIOD_800KHZ 30
    REG_WRITE(`$INSTANCE_NAME`_PWM_DISABLE_FIFO, 0x03);
    REG_WRITE(`$INSTANCE_NAME`_FIFO_LOOP, 0x02);
    REG_WRITE(`$INSTANCE_NAME`_PWM_PERIOD, PERIOD_800KHZ);
    REG_WRITE(`$INSTANCE_NAME`_PWM_COMP0, (PERIOD_800KHZ * 20) / 25);
    REG_WRITE(`$INSTANCE_NAME`_PWM_COMP1, (PERIOD_800KHZ * 12) / 25);
    REG16_WRITE(`$INSTANCE_NAME`_DMA_COUNT, chain_length - 1);
    `$INSTANCE_NAME`_SetBrightness(0x10);
}

void `$INSTANCE_NAME`_SetupIRQ(uint16 chain_length, uint8 **rgb, uint8 *offset,
                uint8 *count) {
    rgb_d = rgb;
    offset_d = offset;
    count_d = count;

    write_udb_constants(chain_length);

    `$INSTANCE_NAME`_DRQ_StartEx(`$INSTANCE_NAME`_isr);

    `$INSTANCE_NAME`_UpdateIRQ();
}

void `$INSTANCE_NAME`_SetupDMA(uint16 chain_length, uint8 *offset,
                uint8 *count) {

    write_udb_constants(chain_length);

#define DMA_CH_INIT(_ch, _name, _size, _prio) \
        dma_ch[_ch] = `$INSTANCE_NAME`_DMA_##_name##_DmaInitialize(_size,   \
            REQ_PER_BURST, DATA_MEM_LOC, UDB_REG_MEM_LOC);                  \
        CyDmaChPriority(dma_ch[_ch], _prio);

    DMA_CH_INIT(0, 0, RGB_SIZE, 0);
    DMA_CH_INIT(1, 1, RGB_SIZE, 0);
    DMA_CH_INIT(2, 2, RGB_SIZE, 0);
    DMA_CH_INIT(3, 3, RGB_SIZE, 0);
    DMA_CH_INIT(4, 4, RGB_SIZE, 0);
    DMA_CH_INIT(5, 5, RGB_SIZE, 0);
    DMA_CH_INIT(6, 6, RGB_SIZE, 0);
    DMA_CH_INIT(7, 7, RGB_SIZE, 0);
    DMA_CH_INIT(8, 8, RGB_SIZE, 0);
    DMA_CH_INIT(9, 9, RGB_SIZE, 0);
    DMA_CH_INIT(10, O, OFFSET_SIZE, 0);
    DMA_CH_INIT(11, C, COUNT_SIZE, 1); /* Must be lower priority */

#define DMA_TD_INIT(_i, _size) \
        td[_i] = CyDmaTdAllocate();                                         \
        CyDmaTdSetConfiguration(td[_i], _size,                              \
            DMA_DISABLE_TD, CY_DMA_TD_INC_SRC_ADR);

    DMA_TD_INIT(0, RGB_SIZE * chain_length);
    DMA_TD_INIT(1, RGB_SIZE * chain_length);
    DMA_TD_INIT(2, RGB_SIZE * chain_length);
    DMA_TD_INIT(3, RGB_SIZE * chain_length);
    DMA_TD_INIT(4, RGB_SIZE * chain_length);
    DMA_TD_INIT(5, RGB_SIZE * chain_length);
    DMA_TD_INIT(6, RGB_SIZE * chain_length);
    DMA_TD_INIT(7, RGB_SIZE * chain_length);
    DMA_TD_INIT(8, RGB_SIZE * chain_length);
    DMA_TD_INIT(9, RGB_SIZE * chain_length);
    DMA_TD_INIT(10, OFFSET_SIZE * chain_length);
    DMA_TD_INIT(11, COUNT_SIZE  * chain_length);
    DMA_TD_ADDRESS(10, offset, `$INSTANCE_NAME`_INACTIVE_COUNT);
    DMA_TD_ADDRESS(11, count, `$INSTANCE_NAME`_ACTIVE_COUNT);
}
