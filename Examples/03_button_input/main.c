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
    mach32_gpio_init_t button = {
        .port = MACH32_BTN_PORT,
        .pin = MACH32_BTN_PIN,
        .mode = MACH32_GPIO_MODE_INPUT,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_LOW,
        .pupd = MACH32_GPIO_PUPD_NONE,
    };

    mach32_board_init();
    mach32_gpio_init(&led);
    mach32_gpio_init(&button);

    while (1) {
        uint8_t pressed = mach32_gpio_read_pin(MACH32_BTN_PORT,
                                               MACH32_BTN_PIN) == 0U;
        mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN, pressed);
    }
}
