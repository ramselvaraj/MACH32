#include "mach32_spi.h"
#include "mach32_rcc.h"

static uint8_t spi_enable_clock(SPI_TypeDef *spi)
{
    if (spi == SPI1) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_SPI1EN);
    } else if (spi == SPI2) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_SPI2EN);
    } else if (spi == SPI3) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_SPI3EN);
    } else if (spi == SPI4) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_SPI4EN);
    } else {
        return 0U;
    }

    return 1U;
}

void mach32_spi_init(const mach32_spi_init_t *init)
{
    if ((init == 0) || (init->instance == 0)
        || (spi_enable_clock(init->instance) == 0U)) {
        return;
    }

    SPI_TypeDef *spi = init->instance;

    spi->CR1 = 0U;
    spi->CR2 = 0U;

    if (init->role == MACH32_SPI_ROLE_MASTER) {
        spi->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
    }

    if ((init->mode == MACH32_SPI_MODE_1)
        || (init->mode == MACH32_SPI_MODE_3)) {
        spi->CR1 |= SPI_CR1_CPHA;
    }

    if ((init->mode == MACH32_SPI_MODE_2)
        || (init->mode == MACH32_SPI_MODE_3)) {
        spi->CR1 |= SPI_CR1_CPOL;
    }

    spi->CR1 |= ((uint32_t)init->baud_div & 7U) << SPI_CR1_BR_Pos;

    if (init->bit_order == MACH32_SPI_LSB_FIRST) {
        spi->CR1 |= SPI_CR1_LSBFIRST;
    }

    spi->CR1 |= SPI_CR1_SPE;
}

uint8_t mach32_spi_transfer_byte(SPI_TypeDef *spi, uint8_t data)
{
    if (spi == 0) {
        return 0U;
    }

    while ((spi->SR & SPI_SR_TXE) == 0U) {
        __NOP();
    }

    *(__IO uint8_t *)&spi->DR = data;

    while ((spi->SR & SPI_SR_RXNE) == 0U) {
        __NOP();
    }

    return *(__IO uint8_t *)&spi->DR;
}

void mach32_spi_write(SPI_TypeDef *spi, const uint8_t *data, size_t length)
{
    if ((spi == 0) || (data == 0)) {
        return;
    }

    for (size_t index = 0U; index < length; ++index) {
        (void)mach32_spi_transfer_byte(spi, data[index]);
    }

    while ((spi->SR & SPI_SR_BSY) != 0U) {
        __NOP();
    }
}

void mach32_spi_read(SPI_TypeDef *spi, uint8_t *data, size_t length)
{
    if ((spi == 0) || (data == 0)) {
        return;
    }

    for (size_t index = 0U; index < length; ++index) {
        data[index] = mach32_spi_transfer_byte(spi, 0xFFU);
    }
}
