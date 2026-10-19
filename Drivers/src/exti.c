#include "mach32_exti.h"
#include "mach32_nvic.h"
#include "mach32_rcc.h"

#include "stm32f4xx.h"

static IRQn_Type exti_irq_for_line(uint8_t line)
{
    if (line <= 4U) {
        return (IRQn_Type)((int32_t)EXTI0_IRQn + line);
    }

    if (line <= 9U) {
        return EXTI9_5_IRQn;
    }

    return EXTI15_10_IRQn;
}

void mach32_exti_init(const mach32_exti_init_t *init)
{
    if ((init == 0) || (init->pin > 15U) || (init->port > MACH32_GPIOH)) {
        return;
    }

    uint32_t line_mask = 1UL << init->pin;
    uint32_t exticr_index = init->pin / 4U;
    uint32_t exticr_shift = (init->pin % 4U) * 4U;
    IRQn_Type irq = exti_irq_for_line(init->pin);

    mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_SYSCFGEN);

    SYSCFG->EXTICR[exticr_index] &= ~(0xFUL << exticr_shift);
    SYSCFG->EXTICR[exticr_index] |= (uint32_t)init->port << exticr_shift;

    EXTI->RTSR &= ~line_mask;
    EXTI->FTSR &= ~line_mask;

    if ((init->trigger == MACH32_EXTI_TRIGGER_RISING)
        || (init->trigger == MACH32_EXTI_TRIGGER_BOTH)) {
        EXTI->RTSR |= line_mask;
    }

    if ((init->trigger == MACH32_EXTI_TRIGGER_FALLING)
        || (init->trigger == MACH32_EXTI_TRIGGER_BOTH)) {
        EXTI->FTSR |= line_mask;
    }

    mach32_exti_clear_pending(init->pin);
    mach32_exti_enable_line(init->pin);
    mach32_nvic_set_priority(irq, init->priority);
    mach32_nvic_enable_irq(irq);
}

void mach32_exti_enable_line(uint8_t line)
{
    if (line <= 15U) {
        EXTI->IMR |= 1UL << line;
    }
}

void mach32_exti_disable_line(uint8_t line)
{
    if (line <= 15U) {
        EXTI->IMR &= ~(1UL << line);
    }
}

uint8_t mach32_exti_is_pending(uint8_t line)
{
    if (line > 15U) {
        return 0U;
    }

    return (EXTI->PR & (1UL << line)) != 0U;
}

void mach32_exti_clear_pending(uint8_t line)
{
    if (line <= 15U) {
        EXTI->PR = 1UL << line;
    }
}
