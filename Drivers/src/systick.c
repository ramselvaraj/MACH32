#include "mach32_systick.h"

#include "stm32f4xx.h"

static volatile uint32_t systick_milliseconds;

void mach32_systick_init(uint32_t core_clock_hz)
{
    (void)core_clock_hz;
    /* TODO(core): Configure LOAD, VAL and CTRL for a 1 ms interrupt. */
}

void SysTick_Handler(void)
{
    /* TODO(core): Increment the millisecond counter. */
}

uint32_t mach32_systick_millis(void)
{
    return systick_milliseconds;
}

void mach32_systick_delay_ms(uint32_t milliseconds)
{
    (void)milliseconds;
    /* TODO(core): Wait until the required number of millisecond ticks passes. */
}

void mach32_systick_delay_us(uint32_t microseconds)
{
    (void)microseconds;
    /* TODO(core): Implement a cycle-based microsecond delay without disturbing SysTick. */
}
