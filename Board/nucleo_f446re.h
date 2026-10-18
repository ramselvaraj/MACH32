/*
 * MACH32 board support — NUCLEO-F446RE pin map
 *
 * The board-specific knowledge lives here so examples stay clean. Sources:
 *   - UM1724 (board user manual)
 *   - STM32F446xx datasheet (alternate-function map)
 */

#ifndef NUCLEO_F446RE_H
#define NUCLEO_F446RE_H

#include "mach32_gpio.h"

/* LD2 — the green user LED — is wired to PA5. */
#define MACH32_LD2_PORT  MACH32_GPIOA
#define MACH32_LD2_PIN   5U

/* The blue USER push button is on PC13. It is active-low: pressed == reads 0. */
#define MACH32_BTN_PORT  MACH32_GPIOC
#define MACH32_BTN_PIN   13U

/* USART2 is routed to the ST-LINK Virtual COM port: TX on PA2, RX on PA3. */
#define MACH32_USART2_TX_PORT  MACH32_GPIOA
#define MACH32_USART2_TX_PIN   2U
#define MACH32_USART2_RX_PORT  MACH32_GPIOA
#define MACH32_USART2_RX_PIN   3U

/* One-shot board bring-up: enable the clocks for the ports we use. */
void mach32_board_init(void);

#endif /* NUCLEO_F446RE_H */
