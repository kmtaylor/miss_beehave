#include <Arduino.h>

static SPI_HandleTypeDef spi_handle[2];
static DMA_HandleTypeDef dma_handle[3];

static uint8_t scl_data[] = {  0x55, 0x55, 0x55, 0x55, 0x54, 0xaa, 0xaa, 0xaa,
                               0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x80 };

static uint8_t sda_data[] = {  0xe1, 0x87, 0xff, 0xe6, 0x01, 0xc3, 0x0c, 0x00,
                               0x00, 0x30, 0x00, 0x0c, 0x00, 0x00, 0xc0 };

static uint8_t rx_data[15];
static uint8_t rx_regs[3];

void as5600_setup(uint8_t scl, uint8_t sda_out, uint8_t sda_in, uint8_t sclk) {
    __HAL_RCC_SPI2_CLK_ENABLE();
    __HAL_RCC_SPI3_CLK_ENABLE();
    __HAL_RCC_SPI2_FORCE_RESET();
    __HAL_RCC_SPI3_FORCE_RESET();
    __HAL_RCC_SPI2_RELEASE_RESET();
    __HAL_RCC_SPI3_RELEASE_RESET();
    __HAL_RCC_DMA1_CLK_ENABLE();
    pinmap_pinout(digitalPinToPinName(scl), PinMap_SPI_MOSI);
    pinmap_pinout(digitalPinToPinName(sda_out), PinMap_SPI_MOSI);
    pinmap_pinout(digitalPinToPinName(sda_in), PinMap_SPI_MISO);
    pinmap_pinout(digitalPinToPinName(sclk), PinMap_SPI_SCLK);

    dma_handle[0].Instance = DMA1_Stream4;
    dma_handle[0].Init.Channel = DMA_CHANNEL_0;
    dma_handle[0].Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_handle[0].Init.PeriphInc = DMA_PINC_DISABLE;
    dma_handle[0].Init.MemInc = DMA_MINC_ENABLE;
    dma_handle[0].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    dma_handle[0].Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_handle[0].Init.Mode = DMA_NORMAL;
    dma_handle[0].Init.Priority = DMA_PRIORITY_LOW;
    dma_handle[0].Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    dma_handle[1].Instance = DMA1_Stream5;
    dma_handle[1].Init.Channel = DMA_CHANNEL_0;
    dma_handle[1].Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_handle[1].Init.PeriphInc = DMA_PINC_DISABLE;
    dma_handle[1].Init.MemInc = DMA_MINC_ENABLE;
    dma_handle[1].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    dma_handle[1].Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_handle[1].Init.Mode = DMA_NORMAL;
    dma_handle[1].Init.Priority = DMA_PRIORITY_LOW;
    dma_handle[1].Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    dma_handle[2].Instance = DMA1_Stream2;
    dma_handle[2].Init.Channel = DMA_CHANNEL_0;
    dma_handle[2].Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_handle[2].Init.PeriphInc = DMA_PINC_DISABLE;
    dma_handle[2].Init.MemInc = DMA_MINC_ENABLE;
    dma_handle[2].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    dma_handle[2].Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_handle[2].Init.Mode = DMA_NORMAL;
    dma_handle[2].Init.Priority = DMA_PRIORITY_LOW;
    dma_handle[2].Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    /* Set SPI clk to 234KHz (I2C clock is 117KHz) */
    spi_handle[0].Instance = SPI2;
    spi_handle[0].Init.Mode = SPI_MODE_MASTER;
    spi_handle[0].Init.Direction = SPI_DIRECTION_2LINES;
    spi_handle[0].Init.DataSize = SPI_DATASIZE_8BIT; 
    spi_handle[0].Init.NSS = SPI_NSS_SOFT; 
    spi_handle[0].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    spi_handle[0].Init.FirstBit = SPI_FIRSTBIT_MSB; 
    spi_handle[0].Init.TIMode = SPI_TIMODE_ENABLE;
    spi_handle[1].Instance = SPI3;
    spi_handle[1].Init.Mode = SPI_MODE_MASTER;
    spi_handle[1].Init.Direction = SPI_DIRECTION_2LINES;
    spi_handle[1].Init.DataSize = SPI_DATASIZE_8BIT; 
    spi_handle[1].Init.NSS = SPI_NSS_SOFT; 
    spi_handle[1].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    spi_handle[1].Init.FirstBit = SPI_FIRSTBIT_MSB; 
    spi_handle[1].Init.TIMode = SPI_TIMODE_ENABLE;

    HAL_DMA_Init(&dma_handle[0]);
    HAL_DMA_Init(&dma_handle[1]);
    HAL_DMA_Init(&dma_handle[2]);
    spi_handle[0].hdmatx = &dma_handle[0];
    spi_handle[1].hdmatx = &dma_handle[1];
    spi_handle[1].hdmarx = &dma_handle[2];
    HAL_SPI_Init(&spi_handle[0]);
    HAL_SPI_Init(&spi_handle[1]);

    HAL_SPI_Transmit_DMA(&spi_handle[0], scl_data, 15);
    HAL_SPI_TransmitReceive_DMA(&spi_handle[1], sda_data, rx_data, 15);
}

static uint16_t as5600_decode(void) {
    int i, j, p;
    uint8_t tmp;

    memset(rx_regs, 0, 3);
    j = (7 << 3) | 2;
    tmp = (rx_data[j/8] ^ 0xff) << (1 + 2);

    for(i = 0, p = 0; p < 3; ) {
        if (i != 8) {
            rx_regs[p] = (rx_regs[p] << 1) | (tmp >> 7);
            i++;
        } else {
            /* Skip ack bit */
            p++;
            i = 0;
        }
        tmp <<= 2; j += 2;
        if (!(j & 7)) tmp = (rx_data[j/8] ^ 0xff) << 1;
    }
}

uint8_t as5600_get_status(void) {
    return rx_regs[0];
}

uint16_t as5600_get_pos(void) {
    return (rx_regs[1] << 8) | rx_regs[2];
}

void as5600_poll(void) {
    as5600_decode();

    HAL_SPI_Abort(&spi_handle[0]);
    HAL_SPI_Abort(&spi_handle[1]);

    HAL_DMA_Start(&dma_handle[0], (uint32_t) scl_data, 
            (uint32_t) &spi_handle[0].Instance->DR, 15);
    HAL_DMA_Start(&dma_handle[1], (uint32_t) sda_data, 
            (uint32_t) &spi_handle[1].Instance->DR, 15);
    HAL_DMA_Start(&dma_handle[2], (uint32_t) &spi_handle[1].Instance->DR,
            (uint32_t) rx_data, 15);

    __HAL_SPI_ENABLE(&spi_handle[0]);
    __HAL_SPI_ENABLE(&spi_handle[1]);

    SET_BIT(spi_handle[1].Instance->CR2, SPI_CR2_RXDMAEN);
    SET_BIT(spi_handle[0].Instance->CR2, SPI_CR2_TXDMAEN);
    SET_BIT(spi_handle[1].Instance->CR2, SPI_CR2_TXDMAEN);
}
