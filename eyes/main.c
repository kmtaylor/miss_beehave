#include <project.h>

static int usb_up;

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
