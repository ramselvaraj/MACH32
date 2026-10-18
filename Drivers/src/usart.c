#include "mach32_usart.h"

void mach32_usart_init(const mach32_usart_init_t *init)
{
    (void)init;
    /* TODO(core):
     * 1. Enable the USART's bus clock.
     * 2. Calculate and write BRR.
     * 3. Configure word length, parity, stop bits, TX and RX in CR1/CR2.
     * 4. Enable the USART.
     */
}

void mach32_usart_write_byte(USART_TypeDef *usart, uint8_t data)
{
    (void)usart;
    (void)data;
    /* TODO(core): Wait for TXE, then write the low byte of DR. */
}

uint8_t mach32_usart_read_byte(USART_TypeDef *usart)
{
    (void)usart;
    /* TODO(core): Wait for RXNE, then read the low byte of DR. */
    return 0U;
}

void mach32_usart_write(USART_TypeDef *usart, const uint8_t *data, size_t length)
{
    (void)usart;
    (void)data;
    (void)length;
    /* TODO(core): Transmit each byte, then wait for TC. */
}

void mach32_usart_enable_rx_interrupt(USART_TypeDef *usart)
{
    (void)usart;
    /* TODO(core): Enable RXNEIE and the matching NVIC interrupt. */
}

void mach32_usart_disable_rx_interrupt(USART_TypeDef *usart)
{
    (void)usart;
    /* TODO(core): Disable RXNEIE and the matching NVIC interrupt. */
}
