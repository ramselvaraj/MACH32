/* MACH32 Serial Peripheral Interface driver. */

#ifndef MACH32_SPI_H
#define MACH32_SPI_H

#include <stddef.h>
#include <stdint.h>

#include "stm32f4xx.h"

typedef enum {
    MACH32_SPI_ROLE_SLAVE,
    MACH32_SPI_ROLE_MASTER,
} mach32_spi_role_t;

typedef enum {
    MACH32_SPI_MODE_0,
    MACH32_SPI_MODE_1,
    MACH32_SPI_MODE_2,
    MACH32_SPI_MODE_3,
} mach32_spi_mode_t;

typedef enum {
    MACH32_SPI_DIV_2,
    MACH32_SPI_DIV_4,
    MACH32_SPI_DIV_8,
    MACH32_SPI_DIV_16,
    MACH32_SPI_DIV_32,
    MACH32_SPI_DIV_64,
    MACH32_SPI_DIV_128,
    MACH32_SPI_DIV_256,
} mach32_spi_baud_div_t;

typedef enum {
    MACH32_SPI_MSB_FIRST,
    MACH32_SPI_LSB_FIRST,
} mach32_spi_bit_order_t;

typedef struct {
    SPI_TypeDef *instance;
    mach32_spi_role_t role;
    mach32_spi_mode_t mode;
    mach32_spi_baud_div_t baud_div;
    mach32_spi_bit_order_t bit_order;
} mach32_spi_init_t;

void mach32_spi_init(const mach32_spi_init_t *init);
uint8_t mach32_spi_transfer_byte(SPI_TypeDef *spi, uint8_t data);
void mach32_spi_write(SPI_TypeDef *spi, const uint8_t *data, size_t length);
void mach32_spi_read(SPI_TypeDef *spi, uint8_t *data, size_t length);

#endif /* MACH32_SPI_H */
