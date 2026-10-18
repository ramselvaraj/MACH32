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
        default:           return 0;
    }
}

void mach32_gpio_init(const mach32_gpio_init_t *init)
{
    if ((init == 0) || (init->pin > 15U)) {
        return;
    }

    GPIO_TypeDef *gpio = gpio_port_base(init->port);

    if (gpio == 0) {
        return;
    }

    /* The port enum value matches its clock-enable bit in RCC->AHB1ENR. */
    mach32_rcc_ahb1_periph_clock_enable(1U << (uint32_t)init->port);

    /* MODER, OSPEEDR and PUPDR use one two-bit field for each pin. */
    uint32_t shift = (uint32_t)init->pin * 2U;

    /* Configure the output type. */
    gpio->OTYPER &= ~(1U << init->pin);
    gpio->OTYPER |= ((uint32_t)init->otype & 1U) << init->pin;

    /* Configure the output edge speed. */
    gpio->OSPEEDR &= ~(3U << shift);
    gpio->OSPEEDR |= ((uint32_t)init->speed & 3U) << shift;

    /* Configure the internal pull-up or pull-down resistor. */
    gpio->PUPDR &= ~(3U << shift);
    gpio->PUPDR |= ((uint32_t)init->pupd & 3U) << shift;

    if (init->mode == MACH32_GPIO_MODE_ALTFN) {
        /* AFR[0] controls pins 0..7. AFR[1] controls pins 8..15. */
        uint32_t afr_index = (uint32_t)init->pin / 8U;
        uint32_t afr_shift = ((uint32_t)init->pin % 8U) * 4U;

        gpio->AFR[afr_index] &= ~(0xFU << afr_shift);
        gpio->AFR[afr_index] |= ((uint32_t)init->altfn & 0xFU) << afr_shift;
    }

    /* Select the pin mode last, after its other settings are ready. */
    gpio->MODER &= ~(3U << shift);
    gpio->MODER |= ((uint32_t)init->mode & 3U) << shift;
}

void mach32_gpio_write_pin(mach32_gpio_port_t port, uint8_t pin, uint8_t state)
{
    GPIO_TypeDef *gpio = gpio_port_base(port);

    if ((gpio == 0) || (pin > 15U)) {
        return;
    }

    if (state == MACH32_GPIO_PIN_SET) {
        /* Bits 0..15 set output pins. */
        gpio->BSRR = 1U << pin;
    } else {
        /* Bits 16..31 reset output pins. */
        gpio->BSRR = 1U << (pin + 16U);
    }
}

void mach32_gpio_toggle_pin(mach32_gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *gpio = gpio_port_base(port);

    if ((gpio == 0) || (pin > 15U)) {
        return;
    }

    gpio->ODR ^= 1U << pin;
}

uint8_t mach32_gpio_read_pin(mach32_gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *gpio = gpio_port_base(port);

    if ((gpio == 0) || (pin > 15U)) {
        return MACH32_GPIO_PIN_RESET;
    }

    return ((gpio->IDR & (1U << pin)) != 0U)
        ? MACH32_GPIO_PIN_SET
        : MACH32_GPIO_PIN_RESET;
}
