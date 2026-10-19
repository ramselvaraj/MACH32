#include "mach32_timer.h"
#include "mach32_rcc.h"

static uint8_t timer_enable_clock(TIM_TypeDef *timer)
{
    if (timer == TIM1) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_TIM1EN);
    } else if (timer == TIM2) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_TIM2EN);
    } else if (timer == TIM3) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_TIM3EN);
    } else if (timer == TIM4) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_TIM4EN);
    } else if (timer == TIM5) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_TIM5EN);
    } else if (timer == TIM6) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_TIM6EN);
    } else if (timer == TIM7) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_TIM7EN);
    } else if (timer == TIM8) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_TIM8EN);
    } else if (timer == TIM9) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_TIM9EN);
    } else if (timer == TIM10) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_TIM10EN);
    } else if (timer == TIM11) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_TIM11EN);
    } else if (timer == TIM12) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_TIM12EN);
    } else if (timer == TIM13) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_TIM13EN);
    } else if (timer == TIM14) {
        mach32_rcc_apb1_periph_clock_enable(RCC_APB1ENR_TIM14EN);
    } else {
        return 0U;
    }

    return 1U;
}

static volatile uint32_t *timer_ccr(TIM_TypeDef *timer,
                                    mach32_timer_channel_t channel)
{
    switch (channel) {
        case MACH32_TIMER_CHANNEL_1: return &timer->CCR1;
        case MACH32_TIMER_CHANNEL_2: return &timer->CCR2;
        case MACH32_TIMER_CHANNEL_3: return &timer->CCR3;
        case MACH32_TIMER_CHANNEL_4: return &timer->CCR4;
        default:                     return 0;
    }
}

void mach32_timer_init(const mach32_timer_init_t *init)
{
    if ((init == 0) || (init->instance == 0)
        || (timer_enable_clock(init->instance) == 0U)) {
        return;
    }

    TIM_TypeDef *timer = init->instance;

    timer->CR1 = 0U;
    timer->PSC = init->prescaler;
    timer->ARR = init->auto_reload;
    timer->CNT = 0U;

    if (init->auto_reload_preload != 0U) {
        timer->CR1 |= TIM_CR1_ARPE;
    }

    timer->EGR = TIM_EGR_UG;
    timer->SR &= ~TIM_SR_UIF;
}

void mach32_timer_start(TIM_TypeDef *timer)
{
    if (timer != 0) {
        timer->CR1 |= TIM_CR1_CEN;
    }
}

void mach32_timer_stop(TIM_TypeDef *timer)
{
    if (timer != 0) {
        timer->CR1 &= ~TIM_CR1_CEN;
    }
}

void mach32_timer_enable_update_interrupt(TIM_TypeDef *timer)
{
    if (timer != 0) {
        timer->DIER |= TIM_DIER_UIE;
    }
}

void mach32_timer_clear_update_flag(TIM_TypeDef *timer)
{
    if (timer != 0) {
        timer->SR &= ~TIM_SR_UIF;
    }
}

void mach32_timer_pwm_init(TIM_TypeDef *timer, mach32_timer_channel_t channel,
                           uint32_t pulse)
{
    if ((timer == 0) || (timer_ccr(timer, channel) == 0)) {
        return;
    }

    switch (channel) {
        case MACH32_TIMER_CHANNEL_1:
            timer->CCMR1 &= ~TIM_CCMR1_OC1M;
            timer->CCMR1 |= (6UL << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
            timer->CCER |= TIM_CCER_CC1E;
            break;
        case MACH32_TIMER_CHANNEL_2:
            timer->CCMR1 &= ~TIM_CCMR1_OC2M;
            timer->CCMR1 |= (6UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;
            timer->CCER |= TIM_CCER_CC2E;
            break;
        case MACH32_TIMER_CHANNEL_3:
            timer->CCMR2 &= ~TIM_CCMR2_OC3M;
            timer->CCMR2 |= (6UL << TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC3PE;
            timer->CCER |= TIM_CCER_CC3E;
            break;
        case MACH32_TIMER_CHANNEL_4:
            timer->CCMR2 &= ~TIM_CCMR2_OC4M;
            timer->CCMR2 |= (6UL << TIM_CCMR2_OC4M_Pos) | TIM_CCMR2_OC4PE;
            timer->CCER |= TIM_CCER_CC4E;
            break;
        default:
            return;
    }

    mach32_timer_pwm_set_pulse(timer, channel, pulse);

    if ((timer == TIM1) || (timer == TIM8)) {
        timer->BDTR |= TIM_BDTR_MOE;
    }

    timer->EGR = TIM_EGR_UG;
}

void mach32_timer_pwm_set_pulse(TIM_TypeDef *timer, mach32_timer_channel_t channel,
                                uint32_t pulse)
{
    if (timer == 0) {
        return;
    }

    volatile uint32_t *compare_register = timer_ccr(timer, channel);

    if (compare_register != 0) {
        *compare_register = pulse;
    }
}
