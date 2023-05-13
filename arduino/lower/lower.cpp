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
static RS485Class roboteq_rs485(Serial1, 18, -1, -1);
static ModbusRTUClientClass roboteq_modbus(roboteq_rs485); 

static void update_roboteq(uint16_t addr, uint16_t val) {
    int status, i, j = 0;

    roboteq_modbus.beginTransmission(1, HOLDING_REGISTERS, addr, 2);
    roboteq_modbus.write(val);
    status = roboteq_modbus.endTransmission(); // blocks

    if (!status) {
        /* No response from roboteq, try and flush out any init data */
        for (i = 0; i < 64; i++) {
            Serial1.write(0);
        }
        do {
            for (i = 0; i < 64; i++) {
                Serial1.read();
            }
            delay(10);
        } while (Serial1.available() && j++ < 10);
    }
}

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
    Serial.begin(115200);
#ifdef MODBUS_MASTER
    //roboteq_modbus.begin(MODBUS_BAUD);
    rc_ibus.begin();
#else
    modbusino_slave.setup(MODBUS_BAUD);
    Serial1.begin(MODBUS_BAUD);
#endif
}


void loop() {
#ifdef MODBUS_MASTER
    uint16_t rc_left_right, rc_forward_back, rc_valid;
    uint16_t left_drive, right_drive;
    /* Read remote control - check for safety, then pass scaled instructions
     * on to roboteq */
#if 0
    rc_ibus.process();
    if (rc_ibus.available()) {
        rc_left_right = rc_ibus.get(1);
        rc_forward_back = rc_ibus.get(4);
        rc_valid = rc_ibus.get(7) && rc_ibus.get(0) &&
                   rc_ibus.get(6) && rc_ibus.get(5);
    } else {
        /* Lost iBus connection to RC module */
        rc_valid = 0;
    }
    if (!rc_valid) {
        /* Peform emergency stop */
        left_drive = 0;
        right_drive = 0;
    } else {
        /* Update roboteq with RC values */
        left_drive = rc_forward_back * rc_left_right;
        right_drive = rc_forward_back * rc_left_right;
        
    }
    //update_roboteq(1, left_drive);
    //update_roboteq(2, right_drive);

#endif
    static int i;
    if (i-- == 0) {
        Serial.print("hello\r\n");
        i = 100000;
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

