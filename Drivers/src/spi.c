#include "mach32_spi.h"

void mach32_spi_init(const mach32_spi_init_t *init)
{
    (void)init;
    /* TODO(core): Enable the SPI clock and configure CR1/CR2 for role,
     * CPOL/CPHA, baud rate and bit order. */
}

uint8_t mach32_spi_transfer_byte(SPI_TypeDef *spi, uint8_t data)
{
    (void)spi;
    (void)data;
    /* TODO(core): Wait for TXE, write DR, wait for RXNE, then read DR. */
    return 0U;
}

void mach32_spi_write(SPI_TypeDef *spi, const uint8_t *data, size_t length)
{
    (void)spi;
    (void)data;
    (void)length;
    /* TODO(core): Transfer each byte and finish only after BSY clears. */
}

void mach32_spi_read(SPI_TypeDef *spi, uint8_t *data, size_t length)
{
    (void)spi;
    (void)data;
    (void)length;
    /* TODO(core): Clock each byte by transmitting a dummy value. */
}
