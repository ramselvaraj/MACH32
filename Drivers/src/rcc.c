#include "mach32_rcc.h"

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

/* ---- Peripheral clock enable/disable ---- */

void mach32_rcc_ahb1_periph_clock_enable(uint32_t periph)
{
    (void)periph;
    RCC->AHB1ENR |= periph;
}

void mach32_rcc_ahb1_periph_clock_disable(uint32_t periph)
{
    (void)periph;
    RCC->AHB1ENR &= ~periph;
}

void mach32_rcc_apb1_periph_clock_enable(uint32_t periph)
{
    (void)periph;
    RCC->APB1ENR |= periph;
}

void mach32_rcc_apb1_periph_clock_disable(uint32_t periph)
{
    (void)periph;

    RCC->APB1ENR &= ~periph;
}

void mach32_rcc_apb2_periph_clock_enable(uint32_t periph)
{
    (void)periph;

    RCC->APB2ENR |= periph;
}

void mach32_rcc_apb2_periph_clock_disable(uint32_t periph)
{
    (void)periph;
    RCC->APB2ENR &= ~periph;
}

/* ---- Peripheral reset ---- */

void mach32_rcc_ahb1_periph_reset(uint32_t periph)
{
    (void)periph;
    RCC->AHB1RSTR |= periph;
    RCC->AHB1RSTR &= ~periph;
}

void mach32_rcc_apb1_periph_reset(uint32_t periph)
{
    (void)periph;
    RCC->APB1RSTR |= periph;
    RCC->APB1RSTR &= ~periph;
}

void mach32_rcc_apb2_periph_reset(uint32_t periph)
{
    (void)periph;
    RCC->APB2RSTR |= periph;
    RCC->APB2RSTR &= ~periph;
}

/* ---- System clock ---- */

void mach32_rcc_system_clock_config(void)
{
    /* The NUCLEO-F446RE receives an 8 MHz external clock from ST-LINK. */
    RCC->CR |= RCC_CR_HSEBYP;
    RCC->CR |= RCC_CR_HSEON;

    while ((RCC->CR & RCC_CR_HSERDY) == 0U) {
        /* Wait until the external clock is stable. */
        __NOP();
    }

    /* PWR must be clocked before its voltage-scaling and over-drive
     * registers can be configured for 180 MHz operation. */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    /* Select voltage scale 1. VOSRDY becomes valid after the PLL starts. */
    PWR->CR &= ~PWR_CR_VOS;
    PWR->CR |= PWR_CR_VOS;

    /* Configure Flash before increasing SYSCLK. */
    FLASH->ACR = FLASH_ACR_LATENCY_5WS
               | FLASH_ACR_PRFTEN
               | FLASH_ACR_ICEN
               | FLASH_ACR_DCEN;

    /* HCLK = SYSCLK / 1. */
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    /* PCLK1 = HCLK / 4 (45 MHz when SYSCLK is 180 MHz). */
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

    /* PCLK2 = HCLK / 2 (90 MHz when SYSCLK is 180 MHz). */
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

    /* Configure the PLL: 8 MHz / 4 * 180 / 2 = 180 MHz. */
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM
                    | RCC_PLLCFGR_PLLN
                    | RCC_PLLCFGR_PLLP
                    | RCC_PLLCFGR_PLLSRC
                    | RCC_PLLCFGR_PLLQ);

    RCC->PLLCFGR |= (4U << RCC_PLLCFGR_PLLM_Pos)
                  | (180U << RCC_PLLCFGR_PLLN_Pos)
                  | RCC_PLLCFGR_PLLSRC_HSE
                  | (7U << RCC_PLLCFGR_PLLQ_Pos);

    /* PLLP uses 0b00 for division by 2, so its cleared field is correct. */

    RCC->CR |= RCC_CR_PLLON;

    while ((RCC->CR & RCC_CR_PLLRDY) == 0U) {
        /* Wait until the PLL locks to the configured frequency. */
        __NOP();
    }

    while ((PWR->CSR & PWR_CSR_VOSRDY) == 0U) {
        /* Wait until voltage scale 1 is active. */
        __NOP();
    }

    /* Start the over-drive circuit after the PLL is ready. */
    PWR->CR |= PWR_CR_ODEN;

    while ((PWR->CSR & PWR_CSR_ODRDY) == 0U) {
        /* Wait until the over-drive circuit is ready. */
        __NOP();
    }

    /* Connect the over-drive circuit to the processor core. */
    PWR->CR |= PWR_CR_ODSWEN;

    while ((PWR->CSR & PWR_CSR_ODSWRDY) == 0U) {
        /* Wait until the over-drive switch is ready. */
        __NOP();
    }

    /* Select the PLL as SYSCLK and wait for hardware confirmation. */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
        /* Wait until the PLL supplies SYSCLK. */
        __NOP();
    }

    SystemCoreClock = 180000000U;
}

uint32_t mach32_rcc_get_system_clock(void)
{
    return SystemCoreClock;
}
