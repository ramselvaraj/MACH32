/*
 * MACH32 — umbrella header
 *
 * Include this one header to pull in the whole driver pack. It also brings in
 * ST's CMSIS device header (stm32f4xx.h), which defines the peripheral structs
 * (GPIO_TypeDef, RCC_TypeDef, ...) and the register bit-mask macros we rely on.
 */

#ifndef MACH32_H
#define MACH32_H

#include <stdint.h>

#include "stm32f4xx.h"

#include "mach32_rcc.h"
#include "mach32_gpio.h"
#include "mach32_systick.h"
#include "mach32_nvic.h"
#include "mach32_exti.h"
#include "mach32_usart.h"
#include "mach32_timer.h"
#include "mach32_spi.h"
#include "mach32_i2c.h"
#include "mach32_adc.h"
#include "mach32_dma.h"
#include "mach32_flash.h"
#include "mach32_crc32.h"
#include "mach32_iwdg.h"
#include "mach32_can.h"

#endif /* MACH32_H */
