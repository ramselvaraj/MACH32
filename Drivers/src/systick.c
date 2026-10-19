#include "mach32_systick.h"

#include "stm32f4xx.h"

static volatile uint32_t systick_milliseconds;
static uint32_t systick_cycles_per_microsecond;

void mach32_systick_init(uint32_t core_clock_hz)
{
    systick_milliseconds = 0U;

    /* Generate one SysTick interrupt every millisecond. */
    SysTick->LOAD = (core_clock_hz / 1000U) - 1U;
    SysTick->VAL = 0U;

    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk
                  | SysTick_CTRL_TICKINT_Msk
                  | SysTick_CTRL_ENABLE_Msk;

    /* DWT supplies a free-running cycle counter for microsecond delays. */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    systick_cycles_per_microsecond = core_clock_hz / 1000000U;
}

void SysTick_Handler(void)
{
    ++systick_milliseconds;
}

uint32_t mach32_systick_millis(void)
{
    return systick_milliseconds;
}

void mach32_systick_delay_ms(uint32_t milliseconds)
{
    uint32_t start = mach32_systick_millis();

    while ((uint32_t)(mach32_systick_millis() - start) < milliseconds) {
        __NOP();
    }
}

void mach32_systick_delay_us(uint32_t microseconds)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t delay_cycles = microseconds * systick_cycles_per_microsecond;

    while ((uint32_t)(DWT->CYCCNT - start) < delay_cycles) {
        __NOP();
    }
}
