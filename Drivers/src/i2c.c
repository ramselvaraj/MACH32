#include "mach32_i2c.h"
#include "mach32_rcc.h"

static uint8_t i2c_enable_clock(I2C_TypeDef *i2c)
{
    if (i2c == I2C1) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_I2C1EN);
    } else if (i2c == I2C2) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_I2C2EN);
    } else if (i2c == I2C3) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_I2C3EN);
    } else {
        return 0U;
    }

    return 1U;
}

static void i2c_clear_address(I2C_TypeDef *i2c)
{
    volatile uint32_t clear_sequence;

    clear_sequence = i2c->SR1;
    clear_sequence = i2c->SR2;
    (void)clear_sequence;
}

static uint8_t i2c_wait_for_address(I2C_TypeDef *i2c)
{
    while ((i2c->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0U) {
        __NOP();
    }

    if ((i2c->SR1 & I2C_SR1_AF) != 0U) {
        i2c->SR1 &= ~I2C_SR1_AF;
        i2c->CR1 |= I2C_CR1_STOP;
        return 0U;
    }

    return 1U;
}

void mach32_i2c_init(const mach32_i2c_init_t *init)
{
    if ((init == 0) || (init->instance == 0)
        || (init->peripheral_clock_hz == 0U) || (init->bus_clock_hz == 0U)
        || (i2c_enable_clock(init->instance) == 0U)) {
        return;
    }

    I2C_TypeDef *i2c = init->instance;
    uint32_t clock_mhz = init->peripheral_clock_hz / 1000000U;

    i2c->CR1 = 0U;
    i2c->CR2 = clock_mhz & 0x3FU;
    i2c->OAR1 = (1UL << 14) | ((uint32_t)init->own_address << 1U);

    if (init->bus_clock_hz <= 100000U) {
        uint32_t ccr = init->peripheral_clock_hz
                     / (2U * init->bus_clock_hz);

        if (ccr < 4U) {
            ccr = 4U;
        }

        i2c->CCR = ccr & I2C_CCR_CCR;
        i2c->TRISE = clock_mhz + 1U;
    } else {
        uint32_t ccr = init->peripheral_clock_hz
                     / (3U * init->bus_clock_hz);

        if (ccr == 0U) {
            ccr = 1U;
        }

        i2c->CCR = I2C_CCR_FS | (ccr & I2C_CCR_CCR);
        i2c->TRISE = ((clock_mhz * 300U) / 1000U) + 1U;
    }

    i2c->CR1 = I2C_CR1_ACK | I2C_CR1_PE;
}

uint8_t mach32_i2c_probe(I2C_TypeDef *i2c, uint8_t address)
{
    if ((i2c == 0) || (address > 0x7FU)) {
        return 0U;
    }

    while ((i2c->SR2 & I2C_SR2_BUSY) != 0U) {
        __NOP();
    }

    i2c->CR1 |= I2C_CR1_START;

    while ((i2c->SR1 & I2C_SR1_SB) == 0U) {
        __NOP();
    }

    i2c->DR = (uint32_t)address << 1U;

    if (i2c_wait_for_address(i2c) == 0U) {
        return 0U;
    }

    i2c_clear_address(i2c);
    i2c->CR1 |= I2C_CR1_STOP;
    return 1U;
}

uint8_t mach32_i2c_master_transmit(I2C_TypeDef *i2c, uint8_t address,
                                   const uint8_t *data, size_t length)
{
    if ((i2c == 0) || (data == 0) || (length == 0U) || (address > 0x7FU)) {
        return 0U;
    }

    while ((i2c->SR2 & I2C_SR2_BUSY) != 0U) {
        __NOP();
    }

    i2c->CR1 |= I2C_CR1_START;

    while ((i2c->SR1 & I2C_SR1_SB) == 0U) {
        __NOP();
    }

    i2c->DR = (uint32_t)address << 1U;

    if (i2c_wait_for_address(i2c) == 0U) {
        return 0U;
    }

    i2c_clear_address(i2c);

    for (size_t index = 0U; index < length; ++index) {
        while ((i2c->SR1 & I2C_SR1_TXE) == 0U) {
            __NOP();
        }

        i2c->DR = data[index];
    }

    while ((i2c->SR1 & I2C_SR1_BTF) == 0U) {
        __NOP();
    }

    i2c->CR1 |= I2C_CR1_STOP;
    return 1U;
}

uint8_t mach32_i2c_master_receive(I2C_TypeDef *i2c, uint8_t address,
                                  uint8_t *data, size_t length)
{
    if ((i2c == 0) || (data == 0) || (length == 0U) || (address > 0x7FU)) {
        return 0U;
    }

    while ((i2c->SR2 & I2C_SR2_BUSY) != 0U) {
        __NOP();
    }

    i2c->CR1 &= ~I2C_CR1_POS;
    i2c->CR1 |= I2C_CR1_ACK | I2C_CR1_START;

    while ((i2c->SR1 & I2C_SR1_SB) == 0U) {
        __NOP();
    }

    i2c->DR = ((uint32_t)address << 1U) | 1U;

    if (i2c_wait_for_address(i2c) == 0U) {
        return 0U;
    }

    if (length == 1U) {
        i2c->CR1 &= ~I2C_CR1_ACK;
        i2c_clear_address(i2c);
        i2c->CR1 |= I2C_CR1_STOP;

        while ((i2c->SR1 & I2C_SR1_RXNE) == 0U) {
            __NOP();
        }

        data[0] = (uint8_t)i2c->DR;
    } else if (length == 2U) {
        i2c->CR1 |= I2C_CR1_POS;
        i2c->CR1 &= ~I2C_CR1_ACK;
        i2c_clear_address(i2c);

        while ((i2c->SR1 & I2C_SR1_BTF) == 0U) {
            __NOP();
        }

        i2c->CR1 |= I2C_CR1_STOP;
        data[0] = (uint8_t)i2c->DR;
        data[1] = (uint8_t)i2c->DR;
    } else {
        size_t index = 0U;
        size_t remaining = length;

        i2c_clear_address(i2c);

        while (remaining > 3U) {
            while ((i2c->SR1 & I2C_SR1_RXNE) == 0U) {
                __NOP();
            }

            data[index++] = (uint8_t)i2c->DR;
            --remaining;
        }

        while ((i2c->SR1 & I2C_SR1_BTF) == 0U) {
            __NOP();
        }

        i2c->CR1 &= ~I2C_CR1_ACK;
        data[index++] = (uint8_t)i2c->DR;

        while ((i2c->SR1 & I2C_SR1_BTF) == 0U) {
            __NOP();
        }

        i2c->CR1 |= I2C_CR1_STOP;
        data[index++] = (uint8_t)i2c->DR;
        data[index] = (uint8_t)i2c->DR;
    }

    i2c->CR1 &= ~I2C_CR1_POS;
    i2c->CR1 |= I2C_CR1_ACK;
    return 1U;
}
