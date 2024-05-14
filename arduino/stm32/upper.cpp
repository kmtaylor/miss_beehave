#include <Arduino.h>
#include <Modbusino.h>

#include <stm32yyxx_ll_tim.h>

#include "pins.h"
#include "i2c.h"

#define MODBUS_BAUD     115200
#define MODBUS_SLAVE    5
#define MODBUS_IDLE     ((uint16_t) -1)

enum modbus_regs_e {
    MB_STEP,
    MB_DIR,
    MB_QUAD,
    MB_I2C,
    MB_REGS_SIZE,
};

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };
static ModbusinoSlave modbusino_slave(MODBUS_SLAVE);
static HardwareTimer system_tick_timer(TIM2);
static HardwareTimer encoder_timer(TIM3);

static void setup_encoder(void) {
    pinmap_pinout(digitalPinToPinName(INP7), PinMap_TIM);
    pinmap_pinout(digitalPinToPinName(INP8), PinMap_TIM);

    LL_TIM_ENCODER_InitTypeDef encoder_config;
    LL_TIM_ENCODER_StructInit(&encoder_config);
    encoder_config.EncoderMode = LL_TIM_ENCODERMODE_X4_TI12;
    LL_TIM_ENCODER_Init(TIM3, &encoder_config);

    encoder_timer.setOverflow(2048);
    encoder_timer.resume();
}

static void setup_pins(void) {
    pinMode(OUT8, OUTPUT);
    pinMode(OUT10, OUTPUT);

    /* Level shifted pins */
    pinMode(LVL0, OUTPUT);
    pinMode(LVL1, OUTPUT);
    pinMode(LVL2, OUTPUT);
    pinMode(LVL3, OUTPUT);
    pinMode(LVL4, OUTPUT);
    pinMode(CPY, OUTPUT);
    pinMode(DIY, OUTPUT);
    pinMode(CPX, OUTPUT);
    pinMode(LVL8, OUTPUT);
    pinMode(LVL9, OUTPUT);
    pinMode(LVL10, OUTPUT);
    pinMode(LVL11, OUTPUT);
    pinMode(LVL12, OUTPUT);
    pinMode(LVL13, OUTPUT);
    pinMode(LVL14, OUTPUT);
    pinMode(LVL15, OUTPUT);

    digitalWrite(LVL0, 0);
    digitalWrite(LVL1, 0);
    digitalWrite(LVL2, 0);
    digitalWrite(LVL3, 0);
    digitalWrite(LVL4, 0);
    digitalWrite(CPY, 0);
    digitalWrite(DIY, 0);
    digitalWrite(CPX, 0);
    digitalWrite(LVL8, 0);
    digitalWrite(LVL9, 0);
    digitalWrite(LVL10, 0);
    digitalWrite(LVL11, 0);
    digitalWrite(LVL12, 0);
    digitalWrite(LVL13, 0);
    digitalWrite(LVL14, 0);
    digitalWrite(LVL15, 0);
}

static void system_tick(void) {
    static int count = 0;
    static int lamp = 0;

    count++;
    /*
    if (count == 99) {
        digitalWrite(LVL4, lamp);
        lamp = !lamp;
        count = 0;
    }
    */

    if (count == 9) {
        i2c_poll();
        count = 0;
    }

    /*
    switch (state) {
        case 0:
            LL_I2C_GenerateStartCondition(I2C1);
            LL_I2C_TransmitData8(I2C1, 0x6C);
            LL_I2C_TransmitData8(I2C1, 0x0C);
    }
    */
}

void setup() {
    setup_pins();
    i2c_setup(OUT3, OUT4);
    setup_encoder();
    modbusino_slave.setup(MODBUS_BAUD);
    system_tick_timer.attachInterrupt(system_tick);
    system_tick_timer.setOverflow(180e3);
    system_tick_timer.resume();
}

#define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
                                && (mb_regs[offset] = MODBUS_IDLE))
void loop() {
    uint16_t mb_val;
    static int i = 0, val = 0;

    mb_regs[MB_QUAD] = encoder_timer.getCount();
    mb_regs[MB_I2C] = i2c_get_data();

    if (modbusino_slave.loop(mb_regs, MB_REGS_SIZE) > 0) {
        MB_ACTION(MB_STEP) {
            digitalWrite(OUT8, mb_val);
        }
        MB_ACTION(MB_DIR) {
            digitalWrite(OUT10, mb_val);
        }
    }
}
