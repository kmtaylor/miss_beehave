#include <Arduino.h>
#include <Modbusino.h>

#include <stm32yyxx_ll_tim.h>

#include "boot.h"
#include "pins.h"
#include "as5600.h"

#define MODBUS_SLAVE    5
#define MODBUS_IDLE     ((uint16_t) -1)

enum modbus_regs_e {
    MB_REBOOT,
    MB_STEP,
    MB_DIR,
    MB_QUAD,
    MB_AS5600_STATUS,
    MB_AS5600_POS,
    MB_REGS_SIZE,
};

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };
static ModbusinoSlave modbusino_slave(MODBUS_SLAVE);
static HardwareTimer system_tick_timer(TIM2);
static HardwareTimer encoder_timer(TIM3);

static void setup_encoder(void) {
    pinmap_pinout(digitalPinToPinName(PIN_INP7), PinMap_TIM);
    pinmap_pinout(digitalPinToPinName(PIN_INP8), PinMap_TIM);

    LL_TIM_ENCODER_InitTypeDef encoder_config;
    LL_TIM_ENCODER_StructInit(&encoder_config);
    encoder_config.EncoderMode = LL_TIM_ENCODERMODE_X4_TI12;
    LL_TIM_ENCODER_Init(TIM3, &encoder_config);

    encoder_timer.setOverflow(2048);
    encoder_timer.resume();
}

static void setup_pins(void) {
    /* Level shifted pins */
    pinMode(PIN_CPX, OUTPUT);
    pinMode(PIN_DIX, OUTPUT);
    pinMode(PIN_CPY, OUTPUT);
    pinMode(PIN_DIY, OUTPUT);
    pinMode(PIN_DIZ, OUTPUT);
    pinMode(PIN_CPZ, OUTPUT);
    pinMode(PIN_CPA, OUTPUT);
    pinMode(PIN_DIA, OUTPUT);
    pinMode(PIN_CPB, OUTPUT);
    pinMode(PIN_DIB, OUTPUT);
    pinMode(PIN_CPC, OUTPUT);
    pinMode(PIN_DIC, OUTPUT);
    pinMode(PIN_VSO, OUTPUT);
    pinMode(PIN_OUT2, OUTPUT);
}

static void system_tick(void) {
    as5600_poll();
}

void setup() {
    setup_pins();
    setup_encoder();
    as5600_setup(PIN_ESTOP, PIN_OUT1, PIN_1X, PIN_10X);
    modbusino_slave.setup(0);
    system_tick_timer.attachInterrupt(system_tick);
    system_tick_timer.setOverflow(60e3); /* 1ms */
    system_tick_timer.resume();
}

#define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
                                && (mb_regs[offset] = MODBUS_IDLE))
void loop() {
    uint16_t mb_val;
    static int i = 0, val = 0;

    mb_regs[MB_QUAD] = encoder_timer.getCount();
    mb_regs[MB_AS5600_STATUS] = as5600_get_status();
    mb_regs[MB_AS5600_POS] = as5600_get_pos();

    if (modbusino_slave.loop(mb_regs, MB_REGS_SIZE) > 0) {
        MB_ACTION(MB_REBOOT) {
            if (mb_val == 9876) {
                BOOTLOADER_FLAG = BOOTLOADER_FLAG_VALUE;
                HAL_NVIC_SystemReset();
            }
        }
        MB_ACTION(MB_STEP) {
            digitalWrite(PIN_CPX, mb_val);
        }
        MB_ACTION(MB_DIR) {
            digitalWrite(PIN_DIX, mb_val);
        }
    }
}
