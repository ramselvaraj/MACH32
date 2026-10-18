#include "mach32_exti.h"

#include "stm32f4xx.h"

void mach32_exti_init(const mach32_exti_init_t *init)
{
    (void)init;
    /* TODO(core):
     * 1. Enable SYSCFG's APB2 clock.
     * 2. Route the selected GPIO port through SYSCFG->EXTICR.
     * 3. Configure rising/falling edge selection.
     * 4. Unmask the line and configure its NVIC IRQ.
     */
}

void mach32_exti_enable_line(uint8_t line)
{
    (void)line;
    /* TODO(core): Set the line in EXTI->IMR. */
}

void mach32_exti_disable_line(uint8_t line)
{
    (void)line;
    /* TODO(core): Clear the line in EXTI->IMR. */
}

uint8_t mach32_exti_is_pending(uint8_t line)
{
    (void)line;
    /* TODO(core): Read the line in EXTI->PR. */
    return 0U;
}

void mach32_exti_clear_pending(uint8_t line)
{
    (void)line;
    /* TODO(core): Clear the pending flag with a write-one-to-clear operation. */
}
