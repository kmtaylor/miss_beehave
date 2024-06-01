#include <Arduino.h>
#include <Modbusino.h>

#include <stm32yyxx_ll_tim.h>

#include "boot.h"
#include "pins.h"
#include "i2c.h"

#define MODBUS_BAUD     115200
#define MODBUS_SLAVE    5
#define MODBUS_IDLE     ((uint16_t) -1)

enum modbus_regs_e {
    MB_REBOOT,
    MB_STEP,
    MB_DIR,
    MB_QUAD,
    MB_I2C_0,
    MB_I2C_1,
    MB_I2C_2,
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
    pinMode(CPX, OUTPUT);
    pinMode(DIX, OUTPUT);
    pinMode(CPY, OUTPUT);
    pinMode(DIY, OUTPUT);
    pinMode(DIZ, OUTPUT);
    pinMode(CPZ, OUTPUT);
    pinMode(CPA, OUTPUT);
    pinMode(DIA, OUTPUT);
    pinMode(CPB, OUTPUT);
    pinMode(DIB, OUTPUT);
    pinMode(CPC, OUTPUT);
    pinMode(DIC, OUTPUT);
    pinMode(VSO, OUTPUT);
    pinMode(OUT2, OUTPUT);
}

static void system_tick(void) {
    static int count = 0;
    static int count2 = 0;
    static int lamp = 0;

    count++;
    if (count == 99) {
        digitalWrite(CPX, 0);
        digitalWrite(DIX, 0);
        digitalWrite(CPY, 0);
        digitalWrite(DIY, 0);
        digitalWrite(DIZ, 0);
        digitalWrite(CPZ, 0);
        digitalWrite(CPA, 0);
        digitalWrite(DIA, 0);
        digitalWrite(CPB, 0);
        digitalWrite(DIB, 0);
        digitalWrite(CPC, 0);
        digitalWrite(DIC, 0);
        digitalWrite(VSO, 0);
        digitalWrite(OUT2, 0);
        switch(lamp) {
            case 0:
                digitalWrite(CPX, 1);
                break;
            case 1:
                digitalWrite(DIX, 1);
                break;
            case 2:
                digitalWrite(CPY, 1);
                break;
            case 3:
                digitalWrite(DIY, 1);
                break;
            case 4:
                digitalWrite(CPZ, 1);
                break;
            case 5:
                digitalWrite(DIZ, 1);
                break;
            case 6:
                digitalWrite(CPA, 1);
                break;
            case 7:
                digitalWrite(DIA, 1);
                break;
            case 8:
                digitalWrite(CPB, 1);
                break;
            case 9:
                digitalWrite(DIB, 1);
                break;
            case 10:
                digitalWrite(CPC, 1);
                break;
            case 11:
                digitalWrite(DIC, 1);
                break;
            case 12:
                digitalWrite(VSO, 1);
                break;
            case 13:
                digitalWrite(OUT2, 1);
                lamp = -1;
                break;
        }
        lamp++;
        count = 0;
    }

    count2++;
    if (count2 == 1) {
        i2c_poll();
        count2 = 0;
    }
}

void setup() {
    setup_pins();
    i2c_setup(ESTOP, OUT1, D1X, D10X);
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
    mb_regs[MB_I2C_0] = i2c_get_data(0);
    mb_regs[MB_I2C_1] = i2c_get_data(1);
    mb_regs[MB_I2C_2] = i2c_get_data(2);

    if (modbusino_slave.loop(mb_regs, MB_REGS_SIZE) > 0) {
        MB_ACTION(MB_REBOOT) {
            if (mb_val == 9876) {
                BOOTLOADER_FLAG = BOOTLOADER_FLAG_VALUE;
                HAL_NVIC_SystemReset();
            }
        }
        MB_ACTION(MB_STEP) {
            digitalWrite(OUT8, mb_val);
        }
        MB_ACTION(MB_DIR) {
            digitalWrite(OUT10, mb_val);
        }
    }
}
