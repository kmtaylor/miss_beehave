#include <project.h>
#include <stdbool.h>

#include "uvc.h"
#include "pixel_map.h"

#define LED_BRIGHTNESS 20

static volatile int sys_tick = 0;
static volatile int raster_busy = false;
static uint8_t rgb_data_a[LED_RASTER_NUM_OUTPUTS*CHAIN_LENGTH*3] = { 0 };
static uint8_t rgb_data_b[LED_RASTER_NUM_OUTPUTS*CHAIN_LENGTH*3] = { 0 };
static uint8_t offsets[CHAIN_LENGTH] = { 0 };
static uint8_t counts[CHAIN_LENGTH] = { [ 0 ... CHAIN_LENGTH-1] = 1 };
static uint8_t *rgb_data_chain_a[] = {
    rgb_data_a + 0*CHAIN_LENGTH*3,
    rgb_data_a + 1*CHAIN_LENGTH*3,
    rgb_data_a + 2*CHAIN_LENGTH*3,
    rgb_data_a + 3*CHAIN_LENGTH*3,
    rgb_data_a + 4*CHAIN_LENGTH*3,
    rgb_data_a + 5*CHAIN_LENGTH*3,
    rgb_data_a + 6*CHAIN_LENGTH*3,
    rgb_data_a + 7*CHAIN_LENGTH*3,
    rgb_data_a + 8*CHAIN_LENGTH*3,
    rgb_data_a + 9*CHAIN_LENGTH*3,
};
static uint8_t *rgb_data_chain_b[] = {
    rgb_data_b + 0*CHAIN_LENGTH*3,
    rgb_data_b + 1*CHAIN_LENGTH*3,
    rgb_data_b + 2*CHAIN_LENGTH*3,
    rgb_data_b + 3*CHAIN_LENGTH*3,
    rgb_data_b + 4*CHAIN_LENGTH*3,
    rgb_data_b + 5*CHAIN_LENGTH*3,
    rgb_data_b + 6*CHAIN_LENGTH*3,
    rgb_data_b + 7*CHAIN_LENGTH*3,
    rgb_data_b + 8*CHAIN_LENGTH*3,
    rgb_data_b + 9*CHAIN_LENGTH*3,
};
static uint8_t *rgb_data = rgb_data_b;
static uint8_t **rgb_data_chain = rgb_data_chain_a;

CY_ISR(raster_finished) {
    LED_TIMER_Enable();
}

CY_ISR(raster_refresh) {
    LED_TIMER_Stop();
    LED_TIMER_RESET_Write(1);
    raster_busy = false;
}

CY_ISR(sys_tick_handler) {
    sys_tick++;
}

int main(void) {
    int max_sum, sum, norm, new_frame = true, frame_time = 0, frame_num = 0;

    /* API is broken and turns on I2C interrupt (15) */
    CySysTickStart(); CyIntDisable(15);
    CySysTickSetReload(BCLK__BUS_CLK__HZ/1000-1);
    CySysTickClear();
    CySysTickSetCallback(0, sys_tick_handler);

    USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);
    max_sum = uvc_init(pixel_map, sizeof(pixel_map)/sizeof(pixel_map[0]));

    LED_FINISHED_StartEx(raster_finished);
    LED_UPDATE_StartEx(raster_refresh);
    LED_TIMER_Init();
    LED_TIMER_WritePeriod(200);

    uvc_test_pattern(rgb_data_a);
    LED_RASTER_SetBrightness(0x10);
    LED_RASTER_SetupDMA(CHAIN_LENGTH, offsets, counts);

    CyGlobalIntEnable;

    for(;;) {
        if (!raster_busy && (new_frame || ((sys_tick - frame_time) > 30))) {
            raster_busy = true;
            new_frame = false;
            frame_time = sys_tick;
            frame_num++;
            frame_num %= 60;
            LED_RASTER_UpdateDMA(rgb_data_chain);
        }
        if ((sum = uvc_loop(rgb_data)) >= 0) {
            while (raster_busy) {}
            new_frame = true;
            if (rgb_data == rgb_data_a) {
                rgb_data_chain = rgb_data_chain_a;
                rgb_data = rgb_data_b;
            } else {
                rgb_data_chain = rgb_data_chain_b;
                rgb_data = rgb_data_a;
            }
            norm = (max_sum << 8) / (sum + 1);
            if (norm > (1 << 23)) norm = (1 << 23);
            norm = ((norm * uvc_get_brightness()) >> 8) + 1;
            if (norm > 255) norm = 255;
            LED_RASTER_SetBrightness(norm);
        }
        STATUS_LED_Write(frame_num < 30);
    }
}
