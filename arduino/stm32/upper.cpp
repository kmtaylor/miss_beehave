#include <Arduino.h>
#include <Wire.h>
#include <Modbusino.h>

#include <stm32yyxx_ll_tim.h>
#include <stm32yyxx_ll_i2c.h>

#include "pins.h"

#define MODBUS_BAUD     115200
#define MODBUS_SLAVE    5
#define MODBUS_IDLE     ((uint16_t) -1)

enum modbus_regs_e {
    MB_STEP,
    MB_DIR,
    MB_QUAD,
    MB_REGS_SIZE,
};

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };
static ModbusinoSlave modbusino_slave(MODBUS_SLAVE);
static HardwareTimer system_tick_timer(TIM2);
static HardwareTimer encoder_timer(TIM3);
//static TwoWire wire_master(OUT4, OUT3);

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

static void setup_i2c(void) {
    //wire_master.begin(1, false, true);
    //LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_9, LL_GPIO_PULL_DOWN);
    //LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_8, LL_GPIO_PULL_DOWN);
    /*
    wire_slave.begin(PC9, PA8);
    LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_9, LL_GPIO_PULL_DOWN);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_8, LL_GPIO_PULL_DOWN);
    */
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    pinmap_pinout(digitalPinToPinName(OUT3), PinMap_I2C_SCL);
    pinmap_pinout(digitalPinToPinName(OUT4), PinMap_I2C_SDA);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_8, LL_GPIO_PULL_UP);
    LL_I2C_DisableClockStretching(I2C1);
    LL_I2C_InitTypeDef i2c_config;
    LL_I2C_StructInit(&i2c_config);
    LL_I2C_Init(I2C1, &i2c_config);
}

static void setup_pins(void) {
    pinMode(OUT8, OUTPUT);
    pinMode(OUT10, OUTPUT);
    //pinMode(OUT3, OUTPUT);
}

static void system_tick(void) {
    static int state = 0;

    //digitalWrite(OUT3, state);
    //state = (state + 1) & 1;

    /*
    wire_master.beginTransmission(0x21);
    wire_master.write(0xA0);
    wire_master.endTransmission();
    */
    switch (state) {
        case 0:
            LL_I2C_GenerateStartCondition(I2C1);
            state++;
            break;
        case 1:
            LL_I2C_TransmitData8(I2C1, 0xA0);
            state++;
            break;
        default:
            LL_I2C_GenerateStopCondition(I2C1);
            state = 0;
            break;
    }
}

void setup() {
    setup_pins();
    setup_i2c();
    setup_encoder();
    modbusino_slave.setup(MODBUS_BAUD);
    system_tick_timer.attachInterrupt(system_tick);
    system_tick_timer.setOverflow(60e3); // 1ms
    system_tick_timer.resume();
}

#define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
                                && (mb_regs[offset] = MODBUS_IDLE))
void loop() {
    uint16_t mb_val;
    static int i = 0, val = 0;

    mb_regs[MB_QUAD] = encoder_timer.getCount();

    if (modbusino_slave.loop(mb_regs, MB_REGS_SIZE) > 0) {
        MB_ACTION(MB_STEP) {
            digitalWrite(OUT8, mb_val);
        }
        MB_ACTION(MB_DIR) {
            digitalWrite(OUT10, mb_val);
        }
    }

    /*
    if (i++ == 10000) { // Approx 15ms
        //digitalWrite(OUT3, val & 1); 
        wire_master.beginTransmission(0x21);
        wire_master.write(0xA0);
        wire_master.endTransmission();
        i = 0;
        val++;
    }
    */
}
