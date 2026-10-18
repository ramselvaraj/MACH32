#include "mach32_timer.h"

void mach32_timer_init(const mach32_timer_init_t *init)
{
    (void)init;
    /* TODO(core): Enable the timer clock and configure PSC, ARR and CR1. */
}

void mach32_timer_start(TIM_TypeDef *timer)
{
    (void)timer;
    /* TODO(core): Set the counter-enable bit in CR1. */
}

void mach32_timer_stop(TIM_TypeDef *timer)
{
    (void)timer;
    /* TODO(core): Clear the counter-enable bit in CR1. */
}

void mach32_timer_enable_update_interrupt(TIM_TypeDef *timer)
{
    (void)timer;
    /* TODO(core): Enable the update interrupt in DIER. */
}

void mach32_timer_clear_update_flag(TIM_TypeDef *timer)
{
    (void)timer;
    /* TODO(core): Clear the update flag in SR. */
}

void mach32_timer_pwm_init(TIM_TypeDef *timer, mach32_timer_channel_t channel,
                           uint32_t pulse)
{
    (void)timer;
    (void)channel;
    (void)pulse;
    /* TODO(core): Configure PWM mode in CCMR, enable the channel in CCER,
     * and write the initial CCR value. */
}

void mach32_timer_pwm_set_pulse(TIM_TypeDef *timer, mach32_timer_channel_t channel,
                                uint32_t pulse)
{
    (void)timer;
    (void)channel;
    (void)pulse;
    /* TODO(core): Write pulse to the selected CCR register. */
}
