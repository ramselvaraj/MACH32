#include "mach32_nvic.h"

void mach32_nvic_enable_irq(IRQn_Type irq)
{
    (void)irq;
    /* TODO(core): Select the ISER word and set the IRQ's bit. */
}

void mach32_nvic_disable_irq(IRQn_Type irq)
{
    (void)irq;
    /* TODO(core): Select the ICER word and set the IRQ's bit. */
}

void mach32_nvic_set_priority(IRQn_Type irq, uint8_t priority)
{
    (void)irq;
    (void)priority;
    /* TODO(core): Shift the priority into the implemented NVIC priority bits. */
}

void mach32_nvic_set_pending(IRQn_Type irq)
{
    (void)irq;
    /* TODO(core): Select the ISPR word and set the IRQ's bit. */
}

void mach32_nvic_clear_pending(IRQn_Type irq)
{
    (void)irq;
    /* TODO(core): Select the ICPR word and set the IRQ's bit. */
}
