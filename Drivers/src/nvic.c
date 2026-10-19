#include "mach32_nvic.h"

static uint8_t nvic_irq_is_valid(IRQn_Type irq)
{
    int32_t irq_number = (int32_t)irq;

    return (irq_number >= 0) && (irq_number <= (int32_t)FMPI2C1_ER_IRQn);
}

void mach32_nvic_enable_irq(IRQn_Type irq)
{
    if (nvic_irq_is_valid(irq) == 0U) {
        return;
    }

    uint32_t irq_number = (uint32_t)irq;
    uint32_t register_index = irq_number / 32U;
    uint32_t bit_position = irq_number % 32U;

    NVIC->ISER[register_index] = 1UL << bit_position;
}

void mach32_nvic_disable_irq(IRQn_Type irq)
{
    if (nvic_irq_is_valid(irq) == 0U) {
        return;
    }

    uint32_t irq_number = (uint32_t)irq;
    uint32_t register_index = irq_number / 32U;
    uint32_t bit_position = irq_number % 32U;

    NVIC->ICER[register_index] = 1UL << bit_position;
}

void mach32_nvic_set_priority(IRQn_Type irq, uint8_t priority)
{
    if (nvic_irq_is_valid(irq) == 0U) {
        return;
    }

    uint8_t priority_mask = (1U << __NVIC_PRIO_BITS) - 1U;
    uint8_t priority_shift = 8U - __NVIC_PRIO_BITS;

    NVIC->IP[(uint32_t)irq] = (priority & priority_mask) << priority_shift;
}

void mach32_nvic_set_pending(IRQn_Type irq)
{
    if (nvic_irq_is_valid(irq) == 0U) {
        return;
    }

    uint32_t irq_number = (uint32_t)irq;
    uint32_t register_index = irq_number / 32U;
    uint32_t bit_position = irq_number % 32U;

    NVIC->ISPR[register_index] = 1UL << bit_position;
}

void mach32_nvic_clear_pending(IRQn_Type irq)
{
    if (nvic_irq_is_valid(irq) == 0U) {
        return;
    }

    uint32_t irq_number = (uint32_t)irq;
    uint32_t register_index = irq_number / 32U;
    uint32_t bit_position = irq_number % 32U;

    NVIC->ICPR[register_index] = 1UL << bit_position;
}
