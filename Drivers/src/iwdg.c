#include "mach32_iwdg.h"

#include "stm32f4xx.h"

#define MACH32_IWDG_UNLOCK_KEY  0x5555U
#define MACH32_IWDG_REFRESH_KEY 0xAAAAU
#define MACH32_IWDG_START_KEY   0xCCCCU

uint8_t mach32_iwdg_start(uint32_t timeout_ms, uint32_t lsi_clock_hz)
{
    static const uint16_t dividers[7] = {4U, 8U, 16U, 32U, 64U, 128U, 256U};

    if ((timeout_ms == 0U) || (lsi_clock_hz == 0U)) {
        return 0U;
    }

    uint8_t prescaler = 0U;
    uint32_t reload = 0U;

    for (prescaler = 0U; prescaler < 7U; ++prescaler) {
        uint64_t ticks = ((uint64_t)timeout_ms * lsi_clock_hz)
                       / ((uint64_t)dividers[prescaler] * 1000U);

        if ((ticks >= 1U) && (ticks <= 4096U)) {
            reload = (uint32_t)ticks - 1U;
            break;
        }
    }

    if (prescaler == 7U) {
        return 0U;
    }

    /* Starting IWDG also starts its LSI clock. The clock must run before the
     * prescaler and reload update flags can clear. */
    IWDG->KR = MACH32_IWDG_START_KEY;
    IWDG->KR = MACH32_IWDG_UNLOCK_KEY;
    IWDG->PR = prescaler;
    IWDG->RLR = reload;

    while ((IWDG->SR & (IWDG_SR_PVU | IWDG_SR_RVU)) != 0U) {
        __NOP();
    }

    IWDG->KR = MACH32_IWDG_REFRESH_KEY;
    return 1U;
}

void mach32_iwdg_refresh(void)
{
    IWDG->KR = MACH32_IWDG_REFRESH_KEY;
}
