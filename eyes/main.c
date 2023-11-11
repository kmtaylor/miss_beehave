#include <project.h>
#include <stdbool.h>

#include "uvc.h"
#include "pixel_map.h"

#define LED_BRIGHTNESS 20

static volatile int sys_tick = 0;
static volatile int raster_busy = false;
static uint8_t rgb_data_a[LED_RASTER_NUM_OUTPUTS*CHAIN_LENGTH*3];
static uint8_t rgb_data_b[LED_RASTER_NUM_OUTPUTS*CHAIN_LENGTH*3];
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

static void test_pattern(uint16_t length) {
    uint16_t i, j, bytes;
    uint8_t pattern[] = {
        0x00, 0xff, 0x00, /* Red */
        0xff, 0x00, 0x00, /* Blue */
        0x00, 0x00, 0xff, /* Green */
        0xff, 0xff, 0xff, /* White */
    };

    for (i = 0; i < LED_RASTER_NUM_OUTPUTS; i++) {
        j = 0;
        while (j < length*3) {
            bytes = (length*3 - j);
            if (bytes > sizeof(pattern)) bytes = sizeof(pattern);
            memcpy(&rgb_data_chain_a[i][j], pattern, bytes);
            j += bytes;
        }
    }
}

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
    int sum, norm, new_frame = true, frame_time = 0, frame_num = 0;

    /* API is broken and turns on I2C interrupt (15) */
    CySysTickStart(); CyIntDisable(15);
    CySysTickSetReload(BCLK__BUS_CLK__HZ/1000-1);
    CySysTickClear();
    CySysTickSetCallback(0, sys_tick_handler);

    USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);
    uvc_init(pixel_map, LED_RASTER_NUM_OUTPUTS*CHAIN_LENGTH);

    LED_FINISHED_StartEx(raster_finished);
    LED_UPDATE_StartEx(raster_refresh);
    LED_TIMER_Init();
    LED_TIMER_WritePeriod(200);

    test_pattern(CHAIN_LENGTH);
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
            norm = CHAIN_LENGTH*LED_RASTER_NUM_OUTPUTS*3*255 / (sum + 1);
            if (norm > 256) norm = 256;
            if (norm < 128) norm = 128;
            LED_RASTER_SetBrightness((norm * LED_BRIGHTNESS) >> 8);
        }
        STATUS_LED_Write(frame_num < 30);
    }
}
