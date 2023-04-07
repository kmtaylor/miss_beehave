#include <Arduino.h>
#include <Modbusino.h>
#include <Wire.h>

#include "irq_task.h"

#define MODBUS_BAUD     115200
#define MODBUS_SLAVE    3
#define MODBUS_IDLE     ((uint16_t) -1)

#define ADC_I2C_ADDR        0x48
#define ADC_NUM_CHANNELS    2

/* Read ADC over I2C when not equal to -1 */
static volatile int8_t adc_ctx = -1;

static void irq_task(void) {
    static uint16_t i;
    static uint8_t val;
    static int8_t adc_channel;

    adc_channel = (adc_channel + 1) % ADC_NUM_CHANNELS;
    adc_ctx = adc_channel;

    if (i-- == 0) {
        digitalWrite(LED_BUILTIN, val);
        val = !val;
        i = IRQ_TASK_RATE_HZ;
    }
}

enum modbus_regs_e {
    MB_LED_STATE = 0,
    MB_ADC_VAL0,
    MB_ADC_VAL1,
    MB_REGS_SIZE,
};

static void modbus_forward(uint8_t *msg, uint8_t msg_length) {
    Serial1.write(msg, msg_length);
}

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };
static ModbusinoSlave modbusino_slave(MODBUS_SLAVE, modbus_forward);

static uint16_t read_adc(uint8_t channel) {
    uint16_t val;

    Wire.beginTransmission(ADC_I2C_ADDR);
    Wire.write(0x00); /* Conversion register */
    Wire.endTransmission();
    Wire.requestFrom(ADC_I2C_ADDR, 2);
    val  = Wire.read() << 4;
    val += Wire.read() >> 4;

    /* Start next conversion */
    Wire.beginTransmission(ADC_I2C_ADDR);
    Wire.write(0x01); /* Configuration register */
    Wire.write(0xC5 | (channel << 4));
    Wire.write(0x83); /* Defaults */
    Wire.endTransmission();

    return val;
}

void setup() {
    modbusino_slave.setup(MODBUS_BAUD);
    Serial1.begin(MODBUS_BAUD);
    irq_task_setup(irq_task);
    Wire.begin();
}

#define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
                                && (mb_regs[offset] = MODBUS_IDLE))

void loop() {
    uint16_t mb_val;
    int8_t adc_channel;

    adc_channel = adc_ctx;
    if (adc_channel >= 0) {
        mb_regs[adc_channel == 0 ? MB_ADC_VAL0 : MB_ADC_VAL1] = 
            read_adc(adc_channel);
        adc_ctx = -1;
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

