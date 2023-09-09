#include <project.h>

#include "uvc.h"

#define CHAIN_LENGTH 512
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
static uint8_t *rgb_data = rgb_data_a;
static uint8_t **rgb_data_chain = rgb_data_chain_a;

/* type (includes rotation), chain quadrant, chain channel */
static uint16_t pixel_map[10*8] = {
    0x039, 0x038, 0x037, 0x036, 0x420, 0x430, 0x00F, 0x00F, 0x00F, 0x00F, 
    0x029, 0x028, 0x027, 0x026, 0x400, 0x410, 0x00F, 0x00F, 0x00F, 0x00F, 
    0x019, 0x018, 0x017, 0x016, 0x421, 0x431, 0x00F, 0x00F, 0x00F, 0x00F, 
    0x009, 0x008, 0x007, 0x006, 0x401, 0x411, 0x00F, 0x00F, 0x00F, 0x00F, 
    0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 
    0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 
    0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 
    0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 
};

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
            memcpy(&rgb_data_chain_b[i][j], pattern, bytes);
            j += bytes;
        }
    }
}

static void one_pixel(uint16_t pixel) {
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
            memcpy(&rgb_data_chain_a[i][j], pattern, bytes);
            memcpy(&rgb_data_chain_b[i][j], pattern, bytes);
            if ((i*CHAIN_LENGTH + j/3) == pixel) {
                memcpy(&rgb_data_chain_a[i][j], colour, 3);
                memcpy(&rgb_data_chain_b[i][j], colour, 3);
            }
            j += bytes;
        }
    }
}

CY_ISR(raster_finished) {
    LED_TIMER_Enable();
}

CY_ISR(raster_refresh) {
    LED_RASTER_UpdateDMA(rgb_data_chain);
    LED_TIMER_Stop();
    LED_TIMER_RESET_Write(1);
}

int main(void) {
    USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);
    uvc_init(pixel_map);

    LED_FINISHED_StartEx(raster_finished);
    LED_UPDATE_StartEx(raster_refresh);
    LED_TIMER_Init();

    //test_pattern(CHAIN_LENGTH);
    //one_pixel(512*9);
    LED_RASTER_SetupDMA(CHAIN_LENGTH, offsets, counts);
    LED_RASTER_UpdateDMA(rgb_data_chain);

    CyGlobalIntEnable;

    for(;;) {
        if (uvc_loop(rgb_data)) {
            //LED_RASTER_UpdateDMA();
        }
    }
}
