#include <Arduino.h>
#include <Modbusino.h>

#define MODBUS_BAUD		    115200
#define MODBUS_SLAVE		    3
#define MODBUS_IDLE		    ((uint16_t) -1)

enum modbus_regs_e {
    LED_STATE = 0,
    MB_REGS_SIZE,
};

static void modbus_forward(uint8_t *msg, uint8_t msg_length) {
    Serial1.write(msg, msg_length);
}

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };
static ModbusinoSlave modbusino_slave(MODBUS_SLAVE, modbus_forward);

void setup() {
    modbusino_slave.setup(MODBUS_BAUD);
    Serial1.begin(MODBUS_BAUD);
}

#define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
				&& (mb_regs[offset] = MODBUS_IDLE))
void loop() {
    uint16_t mb_val;

    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }

    if (modbusino_slave.loop(mb_regs, MB_REGS_SIZE) > 0) {
	MB_ACTION(LED_STATE) {
            digitalWrite(LED_BUILTIN, mb_val);
        }
    }
}

