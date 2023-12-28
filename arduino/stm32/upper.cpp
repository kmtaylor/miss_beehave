#include <Arduino.h>
#include <Modbusino.h>

#include <stm32yyxx_ll_tim.h>

#include "pins.h"

#define MODBUS_BAUD     115200
#define MODBUS_SLAVE    5
#define MODBUS_IDLE     ((uint16_t) -1)

enum modbus_regs_e {
    MB_STEP,
    MB_DIR,
    MB_QUAD_A,
    MB_QUAD_B,
    MB_REGS_SIZE,
};

static uint16_t mb_regs[MB_REGS_SIZE] = { MODBUS_IDLE };
static ModbusinoSlave modbusino_slave(MODBUS_SLAVE);

void setup() {
    pinMode(INP7, INPUT);
    pinMode(INP8, INPUT);
    pinMode(OUT4, OUTPUT);
    pinMode(OUT6, OUTPUT);
    modbusino_slave.setup(MODBUS_BAUD);

    __HAL_RCC_TIM3_CLK_ENABLE();

    LL_GPIO_InitTypeDef gpio_config;
    LL_GPIO_StructInit(&gpio_config);
    gpio_config.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    gpio_config.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_config.Alternate = LL_GPIO_AF_2; 
    LL_GPIO_Init(GPIOC, &gpio_config); 

    LL_TIM_InitTypeDef timer_config;
    LL_TIM_StructInit(&timer_config);
    timer_config.Autoreload = 2048;
    LL_TIM_Init(TIM3, &timer_config);

    LL_TIM_ENCODER_InitTypeDef encoder_config;
    LL_TIM_ENCODER_StructInit(&encoder_config);
    encoder_config.EncoderMode = LL_TIM_ENCODERMODE_X4_TI12;
    LL_TIM_ENCODER_Init(TIM3, &encoder_config);

    LL_TIM_EnableCounter(TIM3);
}

#define MB_ACTION(offset) if (((mb_val = mb_regs[offset]) != MODBUS_IDLE) \
                                && (mb_regs[offset] = MODBUS_IDLE))
void loop() {
    uint16_t mb_val;

    //mb_regs[MB_QUAD_A] = digitalRead(INP7);
    //mb_regs[MB_QUAD_B] = digitalRead(INP8);
    mb_regs[MB_QUAD_A] = LL_TIM_GetCounter(TIM3);

    if (modbusino_slave.loop(mb_regs, MB_REGS_SIZE) > 0) {
        MB_ACTION(MB_STEP) {
            digitalWrite(OUT4, mb_val);
        }
        MB_ACTION(MB_DIR) {
            digitalWrite(OUT6, mb_val);
        }
    }
}
