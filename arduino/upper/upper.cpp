#include <Arduino.h>
#include <Modbusino.h>
#include <iBus.h>

#define MODBUS_BAUD     115200
#define MODBUS_SLAVE    4
#define MODBUS_IDLE     ((uint16_t) -1)

enum modbus_regs_e {
    MB_LED_STATE = 0,
    MB_CHAN_0,
    MB_CHAN_1,
    MB_CHAN_2,
    MB_CHAN_3,
    MB_CHAN_4,
    MB_CHAN_5,
    MB_CHAN_6,
    MB_CHAN_7,
    MB_CHAN_8,
    MB_CHAN_9,
    MB_REGS_SIZE,
};

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };
static ModbusinoSlave modbusino_slave(MODBUS_SLAVE);
static iBus rc_ibus(Serial2);

void setup() {
    modbusino_slave.setup(MODBUS_BAUD);
    pinMode(LED_BUILTIN, OUTPUT);
    rc_ibus.begin();
}

#define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
                                && (mb_regs[offset] = MODBUS_IDLE))

void loop() {
    uint16_t mb_val;

    if (modbusino_slave.loop(mb_regs, MB_REGS_SIZE) > 0) {
        MB_ACTION(MB_LED_STATE) {
            digitalWrite(LED_BUILTIN, mb_val);
        }
    }

    rc_ibus.process();
    if (rc_ibus.available()) {
        mb_regs[MB_CHAN_0] = rc_ibus.get(0);
        mb_regs[MB_CHAN_1] = rc_ibus.get(1);
        mb_regs[MB_CHAN_2] = rc_ibus.get(2);
        mb_regs[MB_CHAN_3] = rc_ibus.get(3);
        mb_regs[MB_CHAN_4] = rc_ibus.get(4);
        mb_regs[MB_CHAN_5] = rc_ibus.get(5);
        mb_regs[MB_CHAN_6] = rc_ibus.get(6);
        mb_regs[MB_CHAN_7] = rc_ibus.get(7);
        mb_regs[MB_CHAN_8] = rc_ibus.get(8);
        mb_regs[MB_CHAN_9] = rc_ibus.get(9);
    }
}

