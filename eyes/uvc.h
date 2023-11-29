#include <cytypes.h>

extern int uvc_loop(uint8_t *buf);
extern void uvc_init(const int16_t *map, int size);
extern void uvc_test_pattern(uint8_t *buf);
extern int uvc_get_brightness(void);

extern uint8 USBFS_DispatchClassRqst_Callback(uint8 interfaceNumber);
