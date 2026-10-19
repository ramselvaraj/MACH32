#include "mach32.h"
#include "nucleo_f446re.h"

void EXTI0_IRQHandler(void)
{
    mach32_nvic_clear_pending(EXTI0_IRQn);
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

    mach32_board_init();
    mach32_gpio_init(&led);
    mach32_systick_init(16000000U);
    mach32_nvic_set_priority(EXTI0_IRQn, 5U);
    mach32_nvic_enable_irq(EXTI0_IRQn);

    while (1) {
        mach32_nvic_set_pending(EXTI0_IRQn);
        mach32_systick_delay_ms(500U);
    }
}
