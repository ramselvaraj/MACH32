/*
 * MACH32 — Reset & Clock Control (RCC) driver
 *
 * The RCC is the gatekeeper of the chip: every peripheral's clock is OFF by
 * default and must be enabled before its registers respond. It also owns the
 * clock tree — the source (HSI/HSE/PLL) and the AHB/APB prescalers.
 *
 * Buses and their peripherals:
 *   AHB1 -> GPIOA..H, DMA1/2, CRC, FLASH
 *   APB1 -> USART2/3, I2C, SPI2/3, TIM2..7, PWR
 *   APB2 -> USART1/6, SPI1, TIM1/9/10/11
 */

#ifndef MACH32_RCC_H
#define MACH32_RCC_H

#include <stdint.h>

/* ---- Peripheral clock enable/disable (periph = RCC_xxxENR_* mask) ---- */
void mach32_rcc_ahb1_periph_clock_enable(uint32_t periph);
void mach32_rcc_ahb1_periph_clock_disable(uint32_t periph);

void mach32_rcc_apb1_periph_clock_enable(uint32_t periph);
void mach32_rcc_apb1_periph_clock_disable(uint32_t periph);

void mach32_rcc_apb2_periph_clock_enable(uint32_t periph);
void mach32_rcc_apb2_periph_clock_disable(uint32_t periph);

/* ---- Peripheral reset (assert then release; periph = RCC_xxxRSTR_* mask) ---- */
void mach32_rcc_ahb1_periph_reset(uint32_t periph);
void mach32_rcc_apb1_periph_reset(uint32_t periph);
void mach32_rcc_apb2_periph_reset(uint32_t periph);

/* ---- System clock ---- */

/*
 * Configure the full clock tree to run at 180 MHz from the 8 MHz HSE input.
 *
 *   HSE 8 MHz -> PLLM=4 (VCO in 2 MHz) -> PLLN=180 (VCO 360 MHz)
 *              -> PLLP=2 (SYSCLK 180 MHz), PLLQ=7 (USB ~51 MHz, see note)
 *   AHB  = 180 MHz (HPRE /1)
 *   APB1 =  45 MHz (PPRE1 /4)   -> TIM2..7 get 2x = 90 MHz
 *   APB2 =  90 MHz (PPRE2 /2)
 *
 * Note: 180 MHz requires the PWR over-drive mode, which this function enables.
 * If you need an exact 48 MHz USB clock, use 168 MHz instead (PLLN=168, PLLQ=7).
 */
void mach32_rcc_system_clock_config(void);

/* Return the current system clock (HCLK) frequency in Hz. */
uint32_t mach32_rcc_get_system_clock(void);

#endif /* MACH32_RCC_H */
