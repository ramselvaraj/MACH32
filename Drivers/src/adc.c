#include "mach32_adc.h"

void mach32_adc_init(const mach32_adc_init_t *init)
{
    (void)init;
    /* TODO(core): Enable the ADC clock and configure resolution, sample time,
     * sequence length/channel and continuous mode. */
}

void mach32_adc_start(ADC_TypeDef *adc)
{
    (void)adc;
    /* TODO(core): Enable the ADC and start a software-triggered conversion. */
}

uint16_t mach32_adc_read(ADC_TypeDef *adc)
{
    (void)adc;
    /* TODO(core): Wait for EOC, then return the conversion data from DR. */
    return 0U;
}
