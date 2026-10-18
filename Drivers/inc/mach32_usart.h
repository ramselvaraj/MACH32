/* MACH32 USART driver. */

#ifndef MACH32_USART_H
#define MACH32_USART_H

#include <stddef.h>
#include <stdint.h>

#include "stm32f4xx.h"

typedef enum {
    MACH32_USART_MODE_TX   = 1U << 0,
    MACH32_USART_MODE_RX   = 1U << 1,
    MACH32_USART_MODE_TXRX = MACH32_USART_MODE_TX | MACH32_USART_MODE_RX,
} mach32_usart_mode_t;

typedef enum {
    MACH32_USART_PARITY_NONE,
    MACH32_USART_PARITY_EVEN,
    MACH32_USART_PARITY_ODD,
} mach32_usart_parity_t;

typedef enum {
    MACH32_USART_STOP_BITS_1,
    MACH32_USART_STOP_BITS_2,
} mach32_usart_stop_bits_t;

typedef struct {
    USART_TypeDef *instance;
    uint32_t peripheral_clock_hz;
    uint32_t baud_rate;
    mach32_usart_mode_t mode;
    mach32_usart_parity_t parity;
    mach32_usart_stop_bits_t stop_bits;
} mach32_usart_init_t;

void mach32_usart_init(const mach32_usart_init_t *init);
void mach32_usart_write_byte(USART_TypeDef *usart, uint8_t data);
uint8_t mach32_usart_read_byte(USART_TypeDef *usart);
void mach32_usart_write(USART_TypeDef *usart, const uint8_t *data, size_t length);
void mach32_usart_enable_rx_interrupt(USART_TypeDef *usart);
void mach32_usart_disable_rx_interrupt(USART_TypeDef *usart);

#endif /* MACH32_USART_H */
