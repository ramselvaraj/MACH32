#include "mach32_i2c.h"

void mach32_i2c_init(const mach32_i2c_init_t *init)
{
    (void)init;
    /* TODO(core): Enable the I2C clock and configure CR2, CCR and TRISE
     * from the peripheral and bus clock frequencies. */
}

uint8_t mach32_i2c_master_transmit(I2C_TypeDef *i2c, uint8_t address,
                                   const uint8_t *data, size_t length)
{
    (void)i2c;
    (void)address;
    (void)data;
    (void)length;
    /* TODO(core): Generate START, send address, clear ADDR, transmit bytes,
     * wait for BTF, then generate STOP. */
    return 0U;
}

uint8_t mach32_i2c_master_receive(I2C_TypeDef *i2c, uint8_t address,
                                  uint8_t *data, size_t length)
{
    (void)i2c;
    (void)address;
    (void)data;
    (void)length;
    /* TODO(core): Implement the STM32F4 receive sequence, including ACK/NACK
     * handling for one-byte, two-byte and multi-byte transfers. */
    return 0U;
}
