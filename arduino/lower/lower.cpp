#include <Arduino.h>
#if MODBUS_MASTER
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
static volatile int8_t roboteq_ctx = -1;

static void irq_task(void) {
    static uint16_t i, j;
    static uint8_t val;
    static int8_t adc_channel;
    static int8_t roboteq_channel;

    adc_channel = (adc_channel + 1) % ADC_NUM_CHANNELS;
    adc_ctx = adc_channel;

    if (j-- == 0) {
        /* Update roboteq every 100ms */
        roboteq_channel = (roboteq_channel + 1) % 2;
        roboteq_ctx = roboteq_channel;
        j = IRQ_TASK_RATE_HZ/10;
    }

    if (i-- == 0) {
        digitalWrite(LED_BUILTIN, val);
        val = !val;
        i = IRQ_TASK_RATE_HZ;
    }
}

#if MODBUS_MASTER
static iBus rc_ibus(Serial2);

static uint16_t crc16(uint8_t *req, uint8_t req_length) {
    uint8_t j;
    uint16_t crc;

    crc = 0xFFFF;
    while (req_length--) {
        crc = crc ^ *req++;
        for (j = 0; j < 8; j++) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = crc >> 1;
        }
    }

    return (crc << 8 | crc >> 8);
}

static void update_roboteq(uint16_t addr, uint16_t val) {
    uint8_t message[] = { 0x01, 0x10,       // Write holding registers to unit 1
                          0x00, 0x00,       // Register address
                          0x00, 0x02, 0x04, // Write two registers (roboteq requirement)
                          0x00, 0x00,       // 4 bytes, first two unused
                          0x00, 0x00,       // Value
                          0x00, 0x00 };     // Checksum

    message[ 2] = addr >> 8;
    message[ 3] = addr & 0xff;
    message[ 9] = val >> 8;
    message[10] = val & 0xff;

    uint16_t crc = crc16(message, 11);
    message[11] = crc >> 8;
    message[12] = crc & 0xff;

    Serial1.write(message, 13);
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
#ifdef MODBUS_MASTER
    Serial.begin(115200);
    Serial1.begin(MODBUS_BAUD);
    rc_ibus.begin();
#else
    modbusino_slave.setup(MODBUS_BAUD);
    Serial1.begin(MODBUS_BAUD);
#endif
}


void loop() {
#ifdef MODBUS_MASTER
    uint8_t rc_valid;
    int8_t roboteq_channel = roboteq_ctx;
    int16_t rc_left_right, rc_forward_back;
    int16_t roboteq_drive[2];

    /* Read remote control - check for safety, then pass scaled instructions
     * on to roboteq */

    rc_ibus.process();
    if (rc_ibus.available()) {
        rc_left_right = rc_ibus.get(1);
        rc_forward_back = rc_ibus.get(3);
        rc_valid = (rc_ibus.get(0) == 1) &&
                   (rc_ibus.get(5) == 2000) && (rc_ibus.get(6) == 2000);
    } else {
        /* Lost iBus connection to RC module */
        rc_valid = 0;
    }
    if (!rc_valid) {
        /* Peform emergency stop */
        roboteq_drive[0] = 0;
        roboteq_drive[1] = 0;
    } else {
        /* Update roboteq with RC values */
        if (rc_left_right > 1500) {
            if (rc_forward_back > 1500) {
                roboteq_drive[0] = (rc_forward_back - 1500);
                roboteq_drive[1] = (rc_forward_back - 1500) - 0.5*(rc_left_right - 1500);
            } else {
                roboteq_drive[0] = (rc_forward_back - 1500);
                roboteq_drive[1] = (rc_forward_back - 1500) + 0.5*(rc_left_right - 1500);
            }
        } else {
            if (rc_forward_back > 1500) {
                roboteq_drive[0] = (rc_forward_back - 1500) + 0.5*(rc_left_right - 1500);
                roboteq_drive[1] = (rc_forward_back - 1500);
            } else {
                roboteq_drive[0] = (rc_forward_back - 1500) - 0.5*(rc_left_right - 1500);
                roboteq_drive[1] = (rc_forward_back - 1500);
            }
        }
    }

    if (roboteq_channel >= 0) {
        update_roboteq(roboteq_channel + 1, roboteq_drive[roboteq_channel]);
        Serial.print("drive 0: ");
        Serial.print(roboteq_drive[0]);
        Serial.print("\t");
        Serial.print("drive 1: ");
        Serial.print(roboteq_drive[1]);
        Serial.print("\r\n");
        roboteq_ctx = -1;
    }

    /*
    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }
    */
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

