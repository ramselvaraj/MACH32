#include "mach32.h"
#include "nucleo_f446re.h"

void EXTI15_10_IRQHandler(void)
{
    static uint32_t last_press_ms;

    if (mach32_exti_is_pending(MACH32_BTN_PIN) != 0U) {
        mach32_exti_clear_pending(MACH32_BTN_PIN);

        uint32_t now = mach32_systick_millis();

        if ((uint32_t)(now - last_press_ms) >= 100U) {
            last_press_ms = now;
            mach32_gpio_toggle_pin(MACH32_LD2_PORT, MACH32_LD2_PIN);
        }
    }
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
    mach32_gpio_init_t button = {
        .port = MACH32_BTN_PORT,
        .pin = MACH32_BTN_PIN,
        .mode = MACH32_GPIO_MODE_INPUT,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_LOW,
        .pupd = MACH32_GPIO_PUPD_NONE,
    };
    mach32_exti_init_t interrupt = {
        .port = MACH32_BTN_PORT,
        .pin = MACH32_BTN_PIN,
        .trigger = MACH32_EXTI_TRIGGER_FALLING,
        .priority = 5U,
    };

    mach32_board_init();
    mach32_gpio_init(&led);
    mach32_gpio_init(&button);
    mach32_systick_init(16000000U);
    mach32_exti_init(&interrupt);

    while (1) {
        __WFI();
    }
}
