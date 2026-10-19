/* MACH32 independent watchdog driver. */

#ifndef MACH32_IWDG_H
#define MACH32_IWDG_H

#include <stdint.h>

/* Start IWDG with an approximate timeout. Return zero for an invalid timeout. */
uint8_t mach32_iwdg_start(uint32_t timeout_ms, uint32_t lsi_clock_hz);
void mach32_iwdg_refresh(void);

#endif /* MACH32_IWDG_H */
