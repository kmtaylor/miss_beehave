#include <project.h>

/* USBFS_ReadOutEP, USBFS_GetEPState, USBFS_GetEPCount, USBFS_EnableOutEP */

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

int uvc_loop(void) {
    static int uvc_up, eof, sum;
    uint16_t count;
    uint8_t data;
    int i;

    if (USBFS_GetConfiguration()) {
        if (!uvc_up) USBFS_EnableOutEP(USBFS_EP1);
        uvc_up = 1;
    } else {
        uvc_up = 0;
    }

    if (USBFS_GetEPState(USBFS_EP1) != USBFS_OUT_BUFFER_FULL) return 0;

    count = USBFS_GetEPCount(USBFS_EP1);

    if (eof) sum = 0;

    for (i = 0; i < count; i++) {
        data = USBFS_ARB_EP_BASE.arbEp[USBFS_EP1].rwDr;
        if (eof && i < 2) continue; // Skip over header
        sum += data;
    }

    eof = (count == 2);

    return eof ? sum : 0;
}

#define UVC_GET_CUR					0x81
#define UVC_GET_MIN					0x82
#define UVC_GET_MAX					0x83
#define UVC_GET_DEF					0x87

uint8_t response[] = {
/* bmHint                                  */ 0x00u, 0x00u,
/* bFormatIndex                            */ 0x00u,
/* bFrameIndex                             */ 0x00u,
/* bFrameInterval                          */ 0x00u, 0x00u, 0x00u, 0x00u,
/* bKeyFrameRate                           */ 0x00u, 0x00u,
/* bPFrameRate                             */ 0x00u, 0x00u,
/* wCompQuality                            */ 0x00u, 0x00u,
/* wCompWindowSize                         */ 0x00u, 0x00u,
/* wDelay                                  */ 0x00u, 0x00u,
/* dwMaxVideoFrameSize                     */ 0x00u, 0x00u, 0x00u, 0x00u,
/* dwMaxPayloadTransferSize                */ 0x00u, 0x00u, 0x00u, 0x00u,
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
            USBFS_currentTD.wCount = sizeof(response);
            USBFS_currentTD.pData = response;
            return USBFS_InitControlRead();
    }

    return 0;
}

static uint8_t uvc_class_set(void) {
    return 0;
}

uint8 USBFS_DispatchClassRqst_Callback(uint8 interfaceNumber) {
    if (USBFS_bmRequestTypeReg & USBFS_RQST_DIR_D2H)
        return uvc_class_get();
    else
        return uvc_class_set();
}
