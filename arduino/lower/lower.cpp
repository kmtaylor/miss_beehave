#include <Arduino.h>
#if MODBUS_MASTER
    #include <ModbusRTUClient.h>
    #include <iBus.h>
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

#if MODBUS_MASTER
static iBus rc_ibus(Serial2);
#else
enum modbus_regs_e {
    MB_LED_STATE = 0,
    MB_ADC_VAL0,
    MB_ADC_VAL1,
    MB_SHIELD_OUT,
    MB_REGS_SIZE,
};

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };

static void modbus_forward(uint8_t *msg, uint8_t msg_length) {
    Serial1.write(msg, msg_length);
}

static ModbusinoSlave modbusino_slave(MODBUS_SLAVE, modbus_forward);
#endif

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    irq_task_setup(irq_task);
    rugged_shield_setup();
#ifdef MODBUS_MASTER
    ModbusRTUClient.begin(MODBUS_BAUD);
    rc_ibus.begin();
#else
    modbusino_slave.setup(MODBUS_BAUD);
    Serial1.begin(MODBUS_BAUD);
#endif
}


void loop() {
#ifdef MODBUS_MASTER
    uint16_t rc_left_right, rc_forward_back, rc_valid;
    /* Read remote control - check for safety, then pass scaled instructions
     * on to roboteq */
    rc_ibus.process();
    if (rc_ibus.available()) {
        rc_left_right = rc_ibus.get(1);
        rc_forward_back = rc_ibus.get(4);
        rc_valid = rc_ibus.get(7) && rc_ibus.get(6) && rc_ibus.get(5);
    } else {
        /* Lost iBus connection to RC module */
        rc_valid = 0;
    }
    if (!rc_valid) {
        /* Peform emergency stop */
    } else {
        /* Update roboteq with RC values */
        ModbusRTUClient.beginTransmission(1, HOLDING_REGISTERS, 1, 2);
        ModbusRTUClient.write(rc_forward_back);
        ModbusRTUClient.endTransmission(); // blocks
        ModbusRTUClient.beginTransmission(1, HOLDING_REGISTERS, 2, 2);
        ModbusRTUClient.write(rc_left_right);
        ModbusRTUClient.endTransmission();
    }
#else
    #define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
                                && (mb_regs[offset] = MODBUS_IDLE))
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

