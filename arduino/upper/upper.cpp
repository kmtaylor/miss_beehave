#include <Arduino.h>
#include <Modbusino.h>

#define MODBUS_BAUD     115200
#define MODBUS_SLAVE    4
#define MODBUS_IDLE     ((uint16_t) -1)

enum modbus_regs_e {
    MB_LED_STATE = 0,
    MB_REGS_SIZE,
};

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };
static ModbusinoSlave modbusino_slave(MODBUS_SLAVE);

void setup() {
    modbusino_slave.setup(MODBUS_BAUD);
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
}

