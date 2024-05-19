#include <Arduino.h>

#include <stm32yyxx_ll_i2c.h>

static int i2c_state = 0;
static uint8_t data0, data1;

void I2C1_EV_IRQHandler(void) {
    if (LL_I2C_IsActiveFlag_SB(I2C1))
        LL_I2C_TransmitData8(I2C1, 0x6C);

    if (LL_I2C_IsActiveFlag_ADDR(I2C1))
        LL_I2C_ClearFlag_ADDR(I2C1);

    //if (LL_I2C_IsActiveFlag_TXE(I2C1))
    //    LL_I2C_TransmitData8(I2C1, 0x0C);

    /*
    switch (i2c_state) {
        case 0:
            if (LL_I2C_IsActiveFlag_SB(I2C1))
                LL_I2C_TransmitData8(I2C1, 0x6C);
            i2c_state++;
            break;

        case 1:
            LL_I2C_ClearFlag_ADDR(I2C1);
            i2c_state++;
            break;

        case 2:
            LL_I2C_TransmitData8(I2C1, 0x0C);
            i2c_state++;
            break;

        case 3:
            LL_I2C_GenerateStartCondition(I2C1);
            i2c_state++;
            break;

        case 4:
            if (LL_I2C_IsActiveFlag_SB(I2C1))
                LL_I2C_TransmitData8(I2C1, 0x6D);
            i2c_state++;
            break;

        case 5:
            LL_I2C_ClearFlag_ADDR(I2C1);
            LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
            i2c_state++;
            break;

        case 6:
            data0 = LL_I2C_ReceiveData8(I2C1);
            LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
            LL_I2C_GenerateStopCondition(I2C1);
            i2c_state++;
            break;

        case 7:
            data1 = LL_I2C_ReceiveData8(I2C1);
            i2c_state++;
            break;
    }
    */
}

void I2C1_ER_IRQHandler(void) {
    if (LL_I2C_IsActiveFlag_AF(I2C1)) {
        LL_I2C_ClearFlag_AF(I2C1);
        //LL_I2C_TransmitData8(I2C1, 0x0C);
        LL_I2C_GenerateStopCondition(I2C1);
    }
}

void i2c_setup(uint8_t scl, uint8_t sda) {
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 2, 0);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
    pinmap_pinout(digitalPinToPinName(scl), PinMap_I2C_SCL);
    pinmap_pinout(digitalPinToPinName(sda), PinMap_I2C_SDA);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_8, LL_GPIO_PULL_UP);
    LL_I2C_DisableClockStretching(I2C1);
    LL_I2C_InitTypeDef i2c_config;
    LL_I2C_StructInit(&i2c_config);
    LL_I2C_Init(I2C1, &i2c_config);
    LL_I2C_EnableIT_TX(I2C1);
    LL_I2C_EnableIT_ERR(I2C1);
}

uint16_t i2c_get_data(void) {
    return (data0 << 8) | data1;
}

void i2c_poll(void) {
    i2c_state = 0;
    LL_I2C_GenerateStartCondition(I2C1);
}
