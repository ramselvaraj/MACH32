#include "mach32_usart.h"
#include "mach32_rcc.h"

static uint8_t usart_enable_clock(USART_TypeDef *usart)
{
    if (usart == USART1) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_USART1EN);
    } else if (usart == USART2) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_USART2EN);
    } else if (usart == USART3) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_USART3EN);
    } else if (usart == UART4) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_UART4EN);
    } else if (usart == UART5) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_UART5EN);
    } else if (usart == USART6) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_USART6EN);
    } else {
        return 0U;
    }

    return 1U;
}

void mach32_usart_init(const mach32_usart_init_t *init)
{
    if ((init == 0) || (init->instance == 0)
        || (init->peripheral_clock_hz == 0U) || (init->baud_rate == 0U)) {
        return;
    }

    USART_TypeDef *usart = init->instance;

    if (usart_enable_clock(usart) == 0U) {
        return;
    }

    /* Disable and clear the USART configuration before initialization. */
    usart->CR1 = 0U;
    usart->CR2 = 0U;
    usart->CR3 = 0U;

    /* With 16-times oversampling, BRR is the rounded clock-to-baud ratio. */
    usart->BRR = (init->peripheral_clock_hz + (init->baud_rate / 2U))
               / init->baud_rate;

    if ((init->mode & MACH32_USART_MODE_TX) != 0U) {
        usart->CR1 |= USART_CR1_TE;
    }

    if ((init->mode & MACH32_USART_MODE_RX) != 0U) {
        usart->CR1 |= USART_CR1_RE;
    }

    if (init->parity != MACH32_USART_PARITY_NONE) {
        /* M selects nine frame bits. One bit is used for parity, so eight
         * data bits remain. */
        usart->CR1 |= USART_CR1_M | USART_CR1_PCE;

        if (init->parity == MACH32_USART_PARITY_ODD) {
            usart->CR1 |= USART_CR1_PS;
        }
    }

    if (init->stop_bits == MACH32_USART_STOP_BITS_2) {
        usart->CR2 |= USART_CR2_STOP_1;
    }

    usart->CR1 |= USART_CR1_UE;
}

void mach32_usart_write_byte(USART_TypeDef *usart, uint8_t data)
{
    if (usart == 0) {
        return;
    }

    while ((usart->SR & USART_SR_TXE) == 0U) {
        __NOP();
    }

    usart->DR = data;
}

uint8_t mach32_usart_read_byte(USART_TypeDef *usart)
{
    if (usart == 0) {
        return 0U;
    }

    while ((usart->SR & USART_SR_RXNE) == 0U) {
        __NOP();
    }

    return (uint8_t)(usart->DR & 0xFFU);
}

void mach32_usart_write(USART_TypeDef *usart, const uint8_t *data, size_t length)
{
    if ((usart == 0) || (data == 0)) {
        return;
    }

    for (size_t index = 0U; index < length; ++index) {
        mach32_usart_write_byte(usart, data[index]);
    }

    while ((usart->SR & USART_SR_TC) == 0U) {
        __NOP();
    }
}
