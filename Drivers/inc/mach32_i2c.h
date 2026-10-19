/* MACH32 Inter-Integrated Circuit driver. */

#ifndef MACH32_I2C_H
#define MACH32_I2C_H

#include <stddef.h>
#include <stdint.h>

#include "stm32f4xx.h"

typedef struct {
    I2C_TypeDef *instance;
    uint32_t peripheral_clock_hz;
    uint32_t bus_clock_hz;
    uint8_t own_address;
} mach32_i2c_init_t;

void mach32_i2c_init(const mach32_i2c_init_t *init);
uint8_t mach32_i2c_probe(I2C_TypeDef *i2c, uint8_t address);
uint8_t mach32_i2c_master_transmit(I2C_TypeDef *i2c, uint8_t address,
                                   const uint8_t *data, size_t length);
uint8_t mach32_i2c_master_receive(I2C_TypeDef *i2c, uint8_t address,
                                  uint8_t *data, size_t length);

#endif /* MACH32_I2C_H */
