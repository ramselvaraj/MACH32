/*
 * MACH32 GPIO driver — general-purpose I/O
 *
 * A GPIO pin is configured by several registers, each packing one field per pin:
 *
 *   MODER   2 bits/pin  mode         (input / output / alternate / analog)
 *   OTYPER  1 bit /pin  output type  (push-pull / open-drain)
 *   OSPEEDR 2 bits/pin  output speed (low / medium / fast / high)
 *   PUPDR   2 bits/pin  pull         (none / up / down)
 *   IDR     1 bit /pin  read the pin's level (input)
 *   ODR     1 bit /pin  write the pin's level (output)
 *   BSRR    2x16 bits    atomic set (low 16) / reset (high 16) — no read-modify-write needed
 *   AFR[2]  4 bits/pin  alternate function number (only in AF mode)
 */

#ifndef MACH32_GPIO_H
#define MACH32_GPIO_H

#include <stdint.h>

/* Pin levels (arguments to mach32_gpio_write_pin). */
#define MACH32_GPIO_PIN_RESET 0U
#define MACH32_GPIO_PIN_SET   1U

/* Ports, in AHB1ENR bit order (GPIOA = bit 0, GPIOB = bit 1, ...). */
typedef enum {
    MACH32_GPIOA = 0,
    MACH32_GPIOB = 1,
    MACH32_GPIOC = 2,
    MACH32_GPIOD = 3,
    MACH32_GPIOE = 4,
    MACH32_GPIOF = 5,
    MACH32_GPIOG = 6,
    MACH32_GPIOH = 7,
} mach32_gpio_port_t;

/* MODER field values (matches the hardware encoding). */
typedef enum {
    MACH32_GPIO_MODE_INPUT  = 0x0,
    MACH32_GPIO_MODE_OUTPUT = 0x1,
    MACH32_GPIO_MODE_ALTFN  = 0x2,
    MACH32_GPIO_MODE_ANALOG = 0x3,
} mach32_gpio_mode_t;

/* OTYPER field values. */
typedef enum {
    MACH32_GPIO_OTYPE_PUSHPULL  = 0x0,
    MACH32_GPIO_OTYPE_OPENDRAIN = 0x1,
} mach32_gpio_otype_t;

/* OSPEEDR field values. */
typedef enum {
    MACH32_GPIO_SPEED_LOW    = 0x0,
    MACH32_GPIO_SPEED_MEDIUM = 0x1,
    MACH32_GPIO_SPEED_FAST   = 0x2,
    MACH32_GPIO_SPEED_HIGH   = 0x3,
} mach32_gpio_speed_t;

/* PUPDR field values. */
typedef enum {
    MACH32_GPIO_PUPD_NONE = 0x0,
    MACH32_GPIO_PUPD_UP   = 0x1,
    MACH32_GPIO_PUPD_DOWN = 0x2,
} mach32_gpio_pupd_t;

/* Everything needed to configure one pin. */
typedef struct {
    mach32_gpio_port_t  port;
    uint8_t             pin;    /* 0..15 */
    mach32_gpio_mode_t  mode;
    mach32_gpio_otype_t otype;
    mach32_gpio_speed_t speed;
    mach32_gpio_pupd_t  pupd;
    uint8_t             altfn;  /* 0..15 — used only when mode == MACH32_GPIO_MODE_ALTFN */
} mach32_gpio_init_t;

void    mach32_gpio_init(const mach32_gpio_init_t *init);
void    mach32_gpio_write_pin(mach32_gpio_port_t port, uint8_t pin, uint8_t state);
void    mach32_gpio_toggle_pin(mach32_gpio_port_t port, uint8_t pin);
uint8_t mach32_gpio_read_pin(mach32_gpio_port_t port, uint8_t pin);

#endif /* MACH32_GPIO_H */
