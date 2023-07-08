#include <project.h>

static int usb_up;

#define SIZE_J 7
#define SIZE_K 8
#define CHAIN_LENGTH (SIZE_J * SIZE_K)
static volatile uint8_t update_pending_range = 0;
static volatile uint8_t update_pending_brightness = 0;
static uint8_t brightness = 0xff;
static uint8_t rgb_data_0[CHAIN_LENGTH*3];
static uint8_t rgb_data_1[CHAIN_LENGTH*3];
static uint8_t rgb_data_2[CHAIN_LENGTH*3];
static uint8_t rgb_data_3[CHAIN_LENGTH*3];
static uint8_t rgb_data_4[CHAIN_LENGTH*3];
static uint8_t rgb_data_5[CHAIN_LENGTH*3];
static uint8_t rgb_data_6[CHAIN_LENGTH*3];

static uint8_t offsets[CHAIN_LENGTH] = {
	1, 2, 2, 2, 2, 2, 2, 2,
	3, 2, 2, 2, 2, 2, 2, 2,
	3, 2, 2, 2, 2, 2, 2, 2,
	3, 2, 2, 2, 2, 2, 2, 2,
	3, 2, 2, 2, 2, 2, 2, 2,
	3, 2, 2, 2, 2, 2, 2, 2,
	3, 2, 2, 2, 2, 2, 2, 2,
};

static uint8_t counts[CHAIN_LENGTH] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

static uint8_t *rgb_data[] = {
	rgb_data_0, rgb_data_1, rgb_data_2, rgb_data_3,
	rgb_data_4, rgb_data_5, rgb_data_6 };


static void test_pattern(uint8_t length) {
    uint8_t i, j, bytes;
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
    //LED_WATCHDOG_RESET_Write(1);
}

CY_ISR(raster_refresh) {
    LED_RASTER_UpdateDMA();
    LED_TIMER_Stop();
    LED_TIMER_RESET_Write(1);
}

static void usb_poll(void) {
    if (USBFS_GetConfiguration()) {
        if (!usb_up) USBFS_CDC_Init();
        usb_up = 1;
    } else {
        usb_up = 0;
    }
}

void print(char *string) {
    static char print_string[64];
    if (!usb_up) return;
    strcpy(print_string, string);
    while (!USBFS_CDCIsReady()) {}
    USBFS_PutData((uint8_t *)print_string, strlen(string));
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
        if (i == 1000000) {
            print("Loop running\n");
            i = 0;
        }

        usb_poll();
        i++;
    }
}
