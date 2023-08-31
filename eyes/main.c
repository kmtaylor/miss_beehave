#include <project.h>

#include "uvc.h"

#define CHAIN_LENGTH 256
static volatile uint8_t update_pending_range = 0;
static volatile uint8_t update_pending_brightness = 0;
static uint16_t brightness = 0;
static uint8_t rgb_data_0[CHAIN_LENGTH*3];
static uint8_t rgb_data_1[CHAIN_LENGTH*3];
static uint8_t rgb_data_2[CHAIN_LENGTH*3];
static uint8_t rgb_data_3[CHAIN_LENGTH*3];
static uint8_t rgb_data_4[CHAIN_LENGTH*3];
static uint8_t rgb_data_5[CHAIN_LENGTH*3];
static uint8_t rgb_data_6[CHAIN_LENGTH*3];
static uint8_t rgb_data_7[CHAIN_LENGTH*3];
static uint8_t rgb_data_8[CHAIN_LENGTH*3];
static uint8_t rgb_data_9[CHAIN_LENGTH*3];

static uint8_t offsets[CHAIN_LENGTH] = { 0 };
static uint8_t counts[CHAIN_LENGTH] = { [ 0 ... CHAIN_LENGTH-1] = 1 };

static uint8_t *rgb_data[] = {
        rgb_data_0, rgb_data_1, rgb_data_2, rgb_data_3, rgb_data_4,
        rgb_data_5, rgb_data_6, rgb_data_7, rgb_data_8, rgb_data_9 };

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
            memcpy(&rgb_data[i][j], pattern, bytes);
            j += bytes;
        }
    }
}

static int one_pixel(void) {
    static uint16_t pixel;
    uint16_t i, j, bytes;
    uint8_t pattern[] = {
        0x00, 0x00, 0x00, /* Black */
    };
    uint8_t colour[] = {
        0x00, 0xff, 0x00, /* Red */
    };


    for (i = 0; i < LED_RASTER_NUM_OUTPUTS; i++) {
        j = 0;
        while (j < CHAIN_LENGTH*3) {
            bytes = (CHAIN_LENGTH*3 - j);
            if (bytes > sizeof(pattern)) bytes = sizeof(pattern);
            memcpy(&rgb_data[i][j], pattern, bytes);
            if ((i*CHAIN_LENGTH + j/3) == pixel) {
                memcpy(&rgb_data[i][j], colour, 3);
            }
            j += bytes;
        }
    }
    pixel++;
    //pixel = 511;
    if (pixel == CHAIN_LENGTH*10) pixel = 0;
    return pixel;
}

CY_ISR(raster_finished) {
    if (update_pending_range) {
        //update_pixel_range();
        update_pending_range = 0;
    }
    if (update_pending_brightness) {
        LED_RASTER_SetBrightness(brightness);
        update_pending_brightness = 0;
    }
    LED_TIMER_Enable();
}

CY_ISR(raster_refresh) {
    LED_RASTER_UpdateDMA();
    LED_TIMER_Stop();
    LED_TIMER_RESET_Write(1);
}

int main(void) {
    int i = 0;

    USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);
    LED_FINISHED_StartEx(raster_finished);
    LED_UPDATE_StartEx(raster_refresh);
    LED_TIMER_Init();

    test_pattern(CHAIN_LENGTH);
    LED_RASTER_SetupDMA(CHAIN_LENGTH, rgb_data, offsets, counts);

    CyGlobalIntEnable;

    for(;;) {
        if (i == 50000) {
            update_pending_brightness = 1;
            brightness = 10;
            if (brightness == 80) brightness = 0;

            i = 0;
        }

        //uvc_loop();
        i++;
    }
}
