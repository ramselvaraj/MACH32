#include "mach32.h"
#include "nucleo_f446re.h"

int main(void)
{
    mach32_gpio_init_t pwm_pin = {
        .port = MACH32_LD2_PORT,
        .pin = MACH32_LD2_PIN,
        .mode = MACH32_GPIO_MODE_ALTFN,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_HIGH,
        .pupd = MACH32_GPIO_PUPD_NONE,
        .altfn = 1U,
    };
    mach32_timer_init_t timer = {
        .instance = TIM2,
        .prescaler = 15U,
        .auto_reload = 999U,
        .auto_reload_preload = 1U,
    };

    mach32_board_init();
    mach32_gpio_init(&pwm_pin);
    mach32_timer_init(&timer);
    mach32_timer_pwm_init(TIM2, MACH32_TIMER_CHANNEL_1, 0U);
    mach32_timer_start(TIM2);
    mach32_systick_init(16000000U);

    while (1) {
        for (uint32_t pulse = 0U; pulse <= 999U; pulse += 5U) {
            mach32_timer_pwm_set_pulse(TIM2, MACH32_TIMER_CHANNEL_1, pulse);
            mach32_systick_delay_ms(5U);
        }

        for (uint32_t pulse = 999U; pulse >= 5U; pulse -= 5U) {
            mach32_timer_pwm_set_pulse(TIM2, MACH32_TIMER_CHANNEL_1, pulse);
            mach32_systick_delay_ms(5U);
        }
    }
}
