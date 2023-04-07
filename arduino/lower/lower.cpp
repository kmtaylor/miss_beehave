#include <Arduino.h>
#include <Modbusino.h>
#include <Wire.h>

#include "irq_task.h"

#define MODBUS_BAUD     115200
#define MODBUS_SLAVE    3
#define MODBUS_IDLE     ((uint16_t) -1)

enum modbus_regs_e {
    MB_LED_STATE = 0,
    MB_ADC_VAL,
    MB_REGS_SIZE,
};

static void modbus_forward(uint8_t *msg, uint8_t msg_length) {
    Serial1.write(msg, msg_length);
}

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };
static ModbusinoSlave modbusino_slave(MODBUS_SLAVE, modbus_forward);

void irq_task(void) {
    static uint16_t i;
    static uint8_t val;

    if (i-- == 0) {
        //digitalWrite(LED_BUILTIN, val);
        val = !val;
        i = 488;
    }
}

void setup() {
    modbusino_slave.setup(MODBUS_BAUD);
    Serial1.begin(MODBUS_BAUD);
    irq_task_setup(irq_task);
    Wire.begin();
}

#define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
                                && (mb_regs[offset] = MODBUS_IDLE))

static uint16_t read_adc(void) {
    uint16_t val;

    Wire.beginTransmission(0x48);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(0x48, 2);
    val  = Wire.read() << 4;
    val += Wire.read() >> 4;

    // Setup next read
    Wire.beginTransmission(0x48);
    Wire.write(0x01);
    Wire.write(0xC5);
    Wire.write(0x83); /* Defaults */
    Wire.endTransmission();

    return val;
}

void loop() {
    uint16_t mb_val;
    static uint16_t i;

    if (i-- == 0) {
        mb_regs[MB_ADC_VAL] = read_adc();
        i = 10000;
    }

    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }

    if (modbusino_slave.loop(mb_regs, MB_REGS_SIZE) > 0) {
        MB_ACTION(MB_LED_STATE) {
            digitalWrite(LED_BUILTIN, mb_val);
        }
    }
}

