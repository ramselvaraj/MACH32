#include "mach32.h"
#include "nucleo_f446re.h"

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

    if (mach32_iwdg_start(1000U, 32000U) == 0U) {
        mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN,
                              MACH32_GPIO_PIN_SET);
        while (1) {
            __NOP();
        }
    }

    for (uint8_t count = 0U; count < 30U; ++count) {
        mach32_gpio_toggle_pin(MACH32_LD2_PORT, MACH32_LD2_PIN);
        mach32_iwdg_refresh();
        mach32_systick_delay_ms(100U);
    }

    mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN,
                          MACH32_GPIO_PIN_SET);

    while (1) {
        __NOP();
    }
}
