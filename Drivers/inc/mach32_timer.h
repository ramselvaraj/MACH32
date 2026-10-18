/* MACH32 general-purpose timer driver. */

#ifndef MACH32_TIMER_H
#define MACH32_TIMER_H

#include <stdint.h>

#include "stm32f4xx.h"

typedef enum {
    MACH32_TIMER_CHANNEL_1 = 1,
    MACH32_TIMER_CHANNEL_2 = 2,
    MACH32_TIMER_CHANNEL_3 = 3,
    MACH32_TIMER_CHANNEL_4 = 4,
} mach32_timer_channel_t;

typedef struct {
    TIM_TypeDef *instance;
    uint16_t prescaler;
    uint32_t auto_reload;
    uint8_t auto_reload_preload;
} mach32_timer_init_t;

void mach32_timer_init(const mach32_timer_init_t *init);
void mach32_timer_start(TIM_TypeDef *timer);
void mach32_timer_stop(TIM_TypeDef *timer);
void mach32_timer_enable_update_interrupt(TIM_TypeDef *timer);
void mach32_timer_clear_update_flag(TIM_TypeDef *timer);
void mach32_timer_pwm_init(TIM_TypeDef *timer, mach32_timer_channel_t channel,
                           uint32_t pulse);
void mach32_timer_pwm_set_pulse(TIM_TypeDef *timer, mach32_timer_channel_t channel,
                                uint32_t pulse);

#endif /* MACH32_TIMER_H */
