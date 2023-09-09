#include <project.h>
#include <stdbool.h>

/* Packets get written into dedicated USB buffer by USB peripheral. When
 * a packet is ready, an interrupt fires which can be checked with
 * USBFS_GetEPState. Can then read the number of bytes with USBFS_GetEPCount.
 *
 * Data must be read byte-by-byte. So use that opportunity to increment sum
 * for power regulation. Create a local version of USBFS_ReadOutEP for that.
 *
 * Everything can just run from the main loop. Interrupts are just used to
 * acknolwedge that packets are ready. Need to enable EP1 whenever
 * USBFS_GetConfiguration() is true (gets reset by bus_reset)
 */

enum packet_state {
    NO_PACKET = 0,
    PACKET_64,
    PACKET_2
};

static uint8_t ep_dma_buf[64];
static uint8_t ep_work_buf[64 + 2];
static uint16_t *pixel_map;

static int uvc_map_address(int idx) {
    int x, y, quant_x, quant_y;
    int map_addr, map_val;
    int chain_idx, chain_quad, chain_type;
    int mem_addr;

    x = idx % 80;
    y = idx / 80;
    quant_x = x / 8;
    quant_y = y / 8;

    map_addr = quant_y*10 + quant_x;
    map_val = pixel_map[map_addr];

    chain_idx = (map_val >> 0) & 0xf;
    chain_quad = (map_val >> 4) & 0xf;
    chain_type = (map_val >> 8) & 0xf;

    mem_addr = (chain_idx * 512);

    if (chain_idx == 0xf) return -1;

    switch (chain_type) {
        case 0:
            mem_addr += 64;
            mem_addr += (chain_quad << 6);
            mem_addr -= (y & 7) << 3;
            if (y % 2) {
                mem_addr += (x % 8);
                mem_addr -= 8;
            } else {
                mem_addr -= (x % 8);
                mem_addr -= 1;
            }
            break;
        case 4:
            mem_addr += 128;
            mem_addr += (chain_quad / 2) * 128;
            mem_addr -= (y % 8) * 16;
            if (y % 2) {
                mem_addr += (chain_quad % 2) * 8;
                mem_addr += (x % 8);
                mem_addr -= 16;
            } else {
                mem_addr -= (chain_quad % 2) * 8;
                mem_addr -= (x % 8);
                mem_addr -= 1;
            }
            break;
    }

    return mem_addr * 3;
}

static void uvc_process_packet(uint8_t *buf, int first, int last) {
    static int rem, x;
    int i, addr, len = 64;
    uint8_t *ep_shift = ep_work_buf + 2;

    if (first) {
        ep_shift += 2; // Ignore header
        len = 62;
        rem = 0;
        x = 0;
    } else if (last) {
        len = 2;
    }

    ep_shift -= rem;

    for (i = 0; i < len - 2 + rem; i += 3) {
        if ((addr = uvc_map_address(x++)) >= 0) {
            buf[addr + 0] = ep_shift[i + 1];
            buf[addr + 1] = ep_shift[i + 2];
            buf[addr + 2] = ep_shift[i + 0];
        }
    }

    rem = (rem + len) % 3;

    if (rem > 1) ep_work_buf[0] = ep_shift[i++];
    if (rem > 0) ep_work_buf[1] = ep_shift[i++];
}

int uvc_loop(uint8_t *buf) {
    static int uvc_up = false;
    static int prev_count = -1;
    uint16_t count;

    if (!USBFS_GetConfiguration()) {
        uvc_up = false;
        return 0;
    }

    /* Setup DMA after bus reset */
    if (!uvc_up) {
        USBFS_ReadOutEP(USBFS_EP1, ep_dma_buf, 64);
        USBFS_EnableOutEP(USBFS_EP1);
        uvc_up = true;
        return 0;
    }

    if (USBFS_GetEPState(USBFS_EP1) == USBFS_OUT_BUFFER_FULL) {
        count = USBFS_GetEPCount(USBFS_EP1);
        memcpy(ep_work_buf + 2, ep_dma_buf, count);
        USBFS_EnableOutEP(USBFS_EP1);
        if (count) {
            uvc_process_packet(buf, prev_count == 2, count == 2);
            prev_count = count;
        }
        return (count == 2) ? 1 : 0;
    }

    return 0; 
}

void uvc_init(uint16_t *map) {
    pixel_map = map;
}

#define UVC_GET_CUR					0x81
#define UVC_GET_MIN					0x82
#define UVC_GET_MAX					0x83
#define UVC_GET_DEF					0x87
#define UVC_SET_CUR					0x01

uint8_t set_control_data[48];
uint8_t get_control_data[48] = {
/* bmHint                                  */ 0x00u, 0x00u,
/* bFormatIndex                            */ 0x00u,
/* bFrameIndex                             */ 0x00u,
/* bFrameInterval                          */ 0x0au, 0x8bu, 0x02u, 0x00u,
/* bKeyFrameRate                           */ 0x00u, 0x00u,
/* bPFrameRate                             */ 0x00u, 0x00u,
/* wCompQuality                            */ 0x00u, 0x00u,
/* wCompWindowSize                         */ 0x00u, 0x00u,
/* wDelay                                  */ 0x00u, 0x00u,
/* dwMaxVideoFrameSize                     */ 0x00u, 0x00u, 0x00u, 0x00u,
/* dwMaxPayloadTransferSize                */ 0x02u, 0x3cu, 0x00u, 0x00u,
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

static uint8_t uvc_class_get(void) {
    //uint8_t RqstRcpt = USBFS_bmRequestTypeReg & USBFS_RQST_RCPT_MASK;
    //uint8_t wValueHi = USBFS_wValueHiReg;
    //uint8_t epNumber = USBFS_wIndexLoReg & USBFS_DIR_UNUSED;

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

static uint8_t uvc_class_set(void) {
    switch (USBFS_bRequestReg) {
        case UVC_SET_CUR:
            USBFS_currentTD.wCount = sizeof(set_control_data);
            USBFS_currentTD.pData = set_control_data;
            return USBFS_InitControlWrite();
    }

    return 0;
}

uint8 USBFS_DispatchClassRqst_Callback(uint8 interfaceNumber) {
    if (USBFS_bmRequestTypeReg & USBFS_RQST_DIR_D2H)
        return uvc_class_get();
    else
        return uvc_class_set();
}
