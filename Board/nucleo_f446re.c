#include "nucleo_f446re.h"

#include "mach32.h"

void mach32_board_init(void)
{
    /* Clock-gate the ports this board uses. Each RCC_AHB1ENR_* bit maps to one
     * GPIO port (GPIOA = bit 0, GPIOC = bit 2, ...). Without this, the GPIO
     * registers below simply do not respond. */
    mach32_rcc_ahb1_periph_clock_enable(RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN);
}
