/* MACH32 Cortex-M4 SysTick time-base driver. */

#ifndef MACH32_SYSTICK_H
#define MACH32_SYSTICK_H

#include <stdint.h>

/* Configure SysTick as a 1 ms time base using the supplied core clock. */
void mach32_systick_init(uint32_t core_clock_hz);

/* Return the number of milliseconds since mach32_systick_init(). */
uint32_t mach32_systick_millis(void);

/* Busy-wait delays. Use delay_us() only for short delays. */
void mach32_systick_delay_ms(uint32_t milliseconds);
void mach32_systick_delay_us(uint32_t microseconds);

#endif /* MACH32_SYSTICK_H */
