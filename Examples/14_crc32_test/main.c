#include "mach32.h"
#include "nucleo_f446re.h"

int main(void)
{
    static const uint8_t test_data[] = "123456789";
    mach32_gpio_init_t led = {
        .port = MACH32_LD2_PORT,
        .pin = MACH32_LD2_PIN,
        .mode = MACH32_GPIO_MODE_OUTPUT,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_LOW,
        .pupd = MACH32_GPIO_PUPD_NONE,
    };
    uint8_t passed = mach32_crc32(test_data, sizeof(test_data) - 1U)
                   == 0xCBF43926UL;

    mach32_board_init();
    mach32_gpio_init(&led);
    mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN, passed);

    while (1) {
        __NOP();
    }
}
