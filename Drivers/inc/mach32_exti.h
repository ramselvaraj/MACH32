/* MACH32 external interrupt/event controller driver. */

#ifndef MACH32_EXTI_H
#define MACH32_EXTI_H

#include <stdint.h>

#include "mach32_gpio.h"

typedef enum {
    MACH32_EXTI_TRIGGER_RISING,
    MACH32_EXTI_TRIGGER_FALLING,
    MACH32_EXTI_TRIGGER_BOTH,
} mach32_exti_trigger_t;

typedef struct {
    mach32_gpio_port_t port;
    uint8_t pin;       /* EXTI line 0..15 */
    mach32_exti_trigger_t trigger;
    uint8_t priority;  /* NVIC priority 0..15 */
} mach32_exti_init_t;

void mach32_exti_init(const mach32_exti_init_t *init);
void mach32_exti_enable_line(uint8_t line);
void mach32_exti_disable_line(uint8_t line);
uint8_t mach32_exti_is_pending(uint8_t line);
void mach32_exti_clear_pending(uint8_t line);

#endif /* MACH32_EXTI_H */
