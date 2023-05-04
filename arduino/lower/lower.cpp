#include <Arduino.h>
#if MODBUS_MASTER
    #include <ModbusRTUClient.h>
#else
    #include <Modbusino.h>
#endif

#include "irq_task.h"
#include "rugged_shield.h"

#define MODBUS_BAUD     115200
#define MODBUS_SLAVE    3
#define MODBUS_IDLE     ((uint16_t) -1)

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
    MB_SHIELD_OUT,
    MB_REGS_SIZE,
};

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };

#if MODBUS_MASTER
#else
static void modbus_forward(uint8_t *msg, uint8_t msg_length) {
    Serial1.write(msg, msg_length);
}

static ModbusinoSlave modbusino_slave(MODBUS_SLAVE, modbus_forward);
#endif

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
#ifdef MODBUS_MASTER
    ModbusRTUClient.begin(MODBUS_BAUD);
#else
    modbusino_slave.setup(MODBUS_BAUD);
#endif
    Serial1.begin(MODBUS_BAUD);
    irq_task_setup(irq_task);
    rugged_shield_setup();
}

#define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
                                && (mb_regs[offset] = MODBUS_IDLE))

void loop() {
    uint16_t mb_val;
    int8_t adc_channel;

    adc_channel = adc_ctx;
    if (adc_channel >= 0) {
        mb_regs[adc_channel == 0 ? MB_ADC_VAL0 : MB_ADC_VAL1] = 
            rugged_shield_read_adc(adc_channel);
        adc_ctx = -1;
    }

    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }

#ifdef MODBUS_MASTER
#else
    if (modbusino_slave.loop(mb_regs, MB_REGS_SIZE) > 0) {
        MB_ACTION(MB_LED_STATE) {
            digitalWrite(LED_BUILTIN, mb_val);
        }

        MB_ACTION(MB_SHIELD_OUT) {
            rugged_shield_write(mb_val);
        }
    }
#endif
}

