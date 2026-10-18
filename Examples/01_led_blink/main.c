/*
 * 01_led_blink
 *
 * The classic first program: blink LD2 (green LED, PA5) forever.
 *
 * Same behaviour as stm32-baremetal/led_blink.c, but routed through the MACH32
 * GPIO + RCC drivers instead of hand-rolled macros.
 */

#include "mach32.h"
#include "nucleo_f446re.h"

/* Busy-wait. A stopgap until lesson 0005 replaces it with a real SysTick delay —
 * this just burns CPU cycles (one 'nop' per loop) at ~16 MHz HSI. */
static void delay(volatile uint32_t count)
{
    while (count--) {
        __asm volatile("nop");
    }
}

int main(void)
{
    mach32_gpio_init_t led = {
        .port  = MACH32_LD2_PORT,
        .pin   = MACH32_LD2_PIN,
        .mode  = MACH32_GPIO_MODE_OUTPUT,
        .otype = MACH32_GPIO_OTYPE_PUSHPULL,
        .speed = MACH32_GPIO_SPEED_LOW,
        .pupd  = MACH32_GPIO_PUPD_NONE,
    };

    mach32_board_init();
    mach32_gpio_init(&led);

    while (1) {
        mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN, MACH32_GPIO_PIN_SET);
        delay(4000000);

        mach32_gpio_write_pin(MACH32_LD2_PORT, MACH32_LD2_PIN, MACH32_GPIO_PIN_RESET);
        delay(4000000);
    }
}
