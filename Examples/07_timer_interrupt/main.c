#include "mach32.h"
#include "nucleo_f446re.h"

void TIM2_IRQHandler(void)
{
    mach32_timer_clear_update_flag(TIM2);
    mach32_gpio_toggle_pin(MACH32_LD2_PORT, MACH32_LD2_PIN);
}

int main(void)
{
    mach32_gpio_init_t led = {
        .port = MACH32_LD2_PORT,
        .pin = MACH32_LD2_PIN,
        .mode = MACH32_GPIO_MODE_OUTPUT,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_LOW,
        .pupd = MACH32_GPIO_PUPD_NONE,
    };
    mach32_timer_init_t timer = {
        .instance = TIM2,
        .prescaler = 15999U,
        .auto_reload = 499U,
        .auto_reload_preload = 1U,
    };

    mach32_board_init();
    mach32_gpio_init(&led);
    mach32_timer_init(&timer);
    mach32_timer_enable_update_interrupt(TIM2);
    mach32_nvic_set_priority(TIM2_IRQn, 5U);
    mach32_nvic_enable_irq(TIM2_IRQn);
    mach32_timer_start(TIM2);

    while (1) {
        __WFI();
    }
}
