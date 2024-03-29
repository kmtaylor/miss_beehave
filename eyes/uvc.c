#include <project.h>
#include <stdbool.h>

static uint8_t ep_dma_buf[64];
static uint8_t ep_work_buf[64 + 2];
static const int16_t *pixel_map;
static int pixel_map_size;
static uint8_t uvc_brightness_cur[2] = { 0 };
static uint8_t uvc_brightness_def[2] = { 0 };

static int uvc_map_address(int idx) {
#ifdef NOOP_PIXMAP
    return idx > 32*48 ? idx*3 + 6144 : idx*3;
#else
    return idx > pixel_map_size ? -1 : pixel_map[idx];
#endif
}

static int uvc_process_packet(uint8_t *buf, int first, int last) {
    static int rem, x, sum;
    int i, addr, len = 64, byte;
    uint8_t *ep_shift = ep_work_buf + 2;

    if (first) {
        ep_shift += 2; // Ignore header
        len = 62;
        rem = 0;
        x = 0;
        sum = 0;
    } else if (last) {
        len = 2;
    }

    ep_shift -= rem;

    for (i = 0; i < len - 2 + rem; i += 3) {
        if ((addr = uvc_map_address(x++)) >= 0) {
            byte = ep_shift[i + 1]; buf[addr + 0] = byte; sum += byte;
            byte = ep_shift[i + 2]; buf[addr + 1] = byte; sum += byte;
            byte = ep_shift[i + 0]; buf[addr + 2] = byte; sum += byte;
        }
    }

    rem = (rem + len) % 3;

    if (rem > 1) ep_work_buf[0] = ep_shift[i++];
    if (rem > 0) ep_work_buf[1] = ep_shift[i++];

    return sum;
}

int uvc_loop(uint8_t *buf) {
    static int uvc_up = false;
    static int prev_count = -1;
    uint16_t count;
    int sum;

    if (!USBFS_GetConfiguration()) {
        uvc_up = false;
        return -1;
    }

    /* Setup DMA after bus reset */
    if (!uvc_up) {
        USBFS_ReadOutEP(USBFS_EP1, ep_dma_buf, 64);
        USBFS_EnableOutEP(USBFS_EP1);
        uvc_up = true;
        return -1;
    }

    if (USBFS_GetEPState(USBFS_EP1) == USBFS_OUT_BUFFER_FULL) {
        count = USBFS_GetEPCount(USBFS_EP1);
        memcpy(ep_work_buf + 2, ep_dma_buf, count);
        USBFS_EnableOutEP(USBFS_EP1);
        if (count) {
            sum = uvc_process_packet(buf, prev_count == 2, count == 2);
            prev_count = count;
        }
        return (count == 2) ? sum : -1;
    }

    return -1; 
}

int uvc_init(const int16_t *map, int size, uint8_t default_brightness) {
    int i, max_sum = 0;

    pixel_map = map;
    pixel_map_size = size;
    uvc_brightness_def[0] = default_brightness;
    uvc_brightness_cur[0] = default_brightness;

    for (i = 0; i < pixel_map_size; i++) {
        if (uvc_map_address(i) >= 0) max_sum += 3*255;
    }
    return max_sum;
}

void uvc_test_pattern(uint8_t *buf) {
    int i, addr;
    uint8_t pattern[] = {
        0x00, 0xff, 0x00, /* Red */
        0xff, 0x00, 0x00, /* Blue */
        0x00, 0x00, 0xff, /* Green */
        0xff, 0xff, 0xff, /* White */
    };

    for (i = 0; i < pixel_map_size; i++) {
        if ((addr = uvc_map_address(i)) >= 0) {
            buf[addr + 0] = pattern[(i%4)*3 + 0];
            buf[addr + 1] = pattern[(i%4)*3 + 1];
            buf[addr + 2] = pattern[(i%4)*3 + 2];
        }
    }
}

int uvc_get_brightness(void) {
    return uvc_brightness_cur[0];
}

#define UVC_GET_CUR					0x81
#define UVC_GET_MIN					0x82
#define UVC_GET_MAX					0x83
#define UVC_GET_DEF					0x87
#define UVC_SET_CUR					0x01
#define UVC_GET_INFO					0x86
#define UVC_GET_RES					0x84

static uint8_t set_control_data[48];
static uint8_t get_control_data[48] = {
/* bmHint                                  */ 0x00u, 0x00u,
/* bFormatIndex                            */ 0x00u,
/* bFrameIndex                             */ 0x00u,
/* bFrameInterval                          */ 0x40u, 0x0du, 0x03u, 0x00u,
/* bKeyFrameRate                           */ 0x00u, 0x00u,
/* bPFrameRate                             */ 0x00u, 0x00u,
/* wCompQuality                            */ 0x00u, 0x00u,
/* wCompWindowSize                         */ 0x00u, 0x00u,
/* wDelay                                  */ 0x00u, 0x00u,
/* dwMaxVideoFrameSize                     */ 0x00u, 0x00u, 0x00u, 0x00u,
/* dwMaxPayloadTransferSize                */ 0x02u, 0x24u, 0x00u, 0x00u,
/* dwClockFrequency                        */ 0x00u, 0x00u, 0x00u, 0x00u,
/* bmFramingInfo                           */ 0x00u,
/* bPreferedVersion                        */ 0x00u,
/* bMinVersion                             */ 0x00u,
/* bMaxVersion                             */ 0x00u,
/* bUsage                                  */ 0x00u,
/* bBitDepthLuma                           */ 0x00u,
/* bmSettings                              */ 0x00u,
/* bMaxNumberOfRefFramesPlus1              */ 0x00u,
/* bmRateControlModes                      */ 0x00u, 0x00u,
/* bmLayoutPerStream                       */ 0x00u, 0x00u, 0x00u, 0x00u,
                                              0x00u, 0x00u, 0x00u, 0x00u,
};

static uint8_t get_info_data[1] = { 0x03 }; /* Set and get requests */
static uint8_t uvc_brightness_min[2] = { 0x00, 0x00 };
static uint8_t uvc_brightness_max[2] = { 0xFF, 0x00 };
static uint8_t uvc_brightness_res[2] = { 0x01, 0x00 };

static uint8_t uvc_class_get_control(uint8_t unit_id, uint8_t selector) {
    switch (USBFS_bRequestReg) {
        case UVC_GET_MIN:
            USBFS_currentTD.wCount = 2;
            USBFS_currentTD.pData = uvc_brightness_min;
            return USBFS_InitControlRead();
        case UVC_GET_MAX:
            USBFS_currentTD.wCount = 2;
            USBFS_currentTD.pData = uvc_brightness_max;
            return USBFS_InitControlRead();
        case UVC_GET_DEF:
            USBFS_currentTD.wCount = 2;
            USBFS_currentTD.pData = uvc_brightness_def;
            return USBFS_InitControlRead();
        case UVC_GET_CUR:
            USBFS_currentTD.wCount = 2;
            USBFS_currentTD.pData = uvc_brightness_cur;
            return USBFS_InitControlRead();
        case UVC_GET_RES:
            USBFS_currentTD.wCount = 2;
            USBFS_currentTD.pData = uvc_brightness_res;
            return USBFS_InitControlRead();
        case UVC_GET_INFO:
            USBFS_currentTD.wCount = 1;
            USBFS_currentTD.pData = get_info_data;
            return USBFS_InitControlRead();
    }
    return 0;
}

static uint8_t uvc_class_set_control(uint8_t unit_id, uint8_t selector) {
    switch (USBFS_bRequestReg) {
        case UVC_SET_CUR:
            USBFS_currentTD.wCount = 2;
            USBFS_currentTD.pData = uvc_brightness_cur;
            return USBFS_InitControlWrite();
    }

    return 0;
}

static uint8_t uvc_class_get_streaming(void) {
    switch (USBFS_bRequestReg) {
        case UVC_GET_MIN:
        case UVC_GET_MAX:
        case UVC_GET_DEF:
        case UVC_GET_CUR:
            USBFS_currentTD.wCount = sizeof(get_control_data);
            USBFS_currentTD.pData = get_control_data;
            return USBFS_InitControlRead();
    }

    return 0;
}

static uint8_t uvc_class_set_streaming(void) {
    switch (USBFS_bRequestReg) {
        case UVC_SET_CUR:
            USBFS_currentTD.wCount = sizeof(set_control_data);
            USBFS_currentTD.pData = set_control_data;
            return USBFS_InitControlWrite();
    }

    return 0;
}

uint8 USBFS_DispatchClassRqst_Callback(uint8 interfaceNumber) {
    if (USBFS_wIndexLoReg == 1) { // Streaming interface
        if (USBFS_bmRequestTypeReg & USBFS_RQST_DIR_D2H)
            return uvc_class_get_streaming();
        else
            return uvc_class_set_streaming();
    }
    if (USBFS_wIndexLoReg == 0) { // Control interface
        if (USBFS_bmRequestTypeReg & USBFS_RQST_DIR_D2H)
            return uvc_class_get_control(USBFS_wIndexHiReg, USBFS_wValueHiReg);
        else
            return uvc_class_set_control(USBFS_wIndexHiReg, USBFS_wValueHiReg);
    }
    return 0;
}
