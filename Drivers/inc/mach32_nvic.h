/* MACH32 Nested Vectored Interrupt Controller driver. */

#ifndef MACH32_NVIC_H
#define MACH32_NVIC_H

#include <stdint.h>

#include "stm32f4xx.h"

void mach32_nvic_enable_irq(IRQn_Type irq);
void mach32_nvic_disable_irq(IRQn_Type irq);
void mach32_nvic_set_priority(IRQn_Type irq, uint8_t priority);
void mach32_nvic_set_pending(IRQn_Type irq);
void mach32_nvic_clear_pending(IRQn_Type irq);

#endif /* MACH32_NVIC_H */
