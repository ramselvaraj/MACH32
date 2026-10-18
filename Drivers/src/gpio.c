#include "mach32_gpio.h"
#include "mach32_rcc.h"

#include "stm32f4xx.h"

/* Map our port enum to ST's peripheral struct. The enum values line up with the
 * AHB1 clock-enable bit positions, so this also drives the RCC enable below. */
static GPIO_TypeDef *gpio_port_base(mach32_gpio_port_t port)
{
    switch (port) {
        case MACH32_GPIOA: return GPIOA;
        case MACH32_GPIOB: return GPIOB;
        case MACH32_GPIOC: return GPIOC;
        case MACH32_GPIOD: return GPIOD;
        case MACH32_GPIOE: return GPIOE;
        case MACH32_GPIOF: return GPIOF;
        case MACH32_GPIOG: return GPIOG;
        case MACH32_GPIOH: return GPIOH;
        default:           return GPIOA;
    }
}

void mach32_gpio_init(const mach32_gpio_init_t *init)
{
    if (init == 0) {
        return;
    }

    GPIO_TypeDef *gpio = gpio_port_base(init->port);
    (void)gpio;

    /* TODO(core):
     * 1. Enable the selected port's AHB1 clock.
     * 2. Configure MODER, OTYPER, OSPEEDR and PUPDR.
     * 3. If mode is ALTFN, configure the correct AFR register and field.
     */
}

void mach32_gpio_write_pin(mach32_gpio_port_t port, uint8_t pin, uint8_t state)
{
    GPIO_TypeDef *gpio = gpio_port_base(port);
    (void)gpio;
    (void)pin;
    (void)state;
    /* TODO(core): Use BSRR to atomically set or reset the requested pin. */
}

void mach32_gpio_toggle_pin(mach32_gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *gpio = gpio_port_base(port);
    (void)gpio;
    (void)pin;
    /* TODO(core): Toggle the requested bit in ODR. */
}

uint8_t mach32_gpio_read_pin(mach32_gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *gpio = gpio_port_base(port);
    (void)gpio;
    (void)pin;
    /* TODO(core): Read and mask the requested bit in IDR. */
    return MACH32_GPIO_PIN_RESET;
}
