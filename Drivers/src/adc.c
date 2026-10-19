#include "mach32_adc.h"
#include "mach32_rcc.h"

static uint8_t adc_enable_clock(ADC_TypeDef *adc)
{
    if (adc == ADC1) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_ADC1EN);
    } else if (adc == ADC2) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_ADC2EN);
    } else if (adc == ADC3) {
        mach32_rcc_apb2_periph_clock_enable(RCC_APB2ENR_ADC3EN);
    } else {
        return 0U;
    }

    return 1U;
}

void mach32_adc_init(const mach32_adc_init_t *init)
{
    if ((init == 0) || (init->instance == 0) || (init->channel > 18U)
        || (adc_enable_clock(init->instance) == 0U)) {
        return;
    }

    ADC_TypeDef *adc = init->instance;

    /* APB2 is 90 MHz. Divide it by four for a 22.5 MHz ADC clock. */
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= ADC_CCR_ADCPRE_0;

    adc->CR1 = ((uint32_t)init->resolution & 3U) << ADC_CR1_RES_Pos;
    adc->CR2 = 0U;

    if (init->continuous != 0U) {
        adc->CR2 |= ADC_CR2_CONT;
    }

    if (init->channel <= 9U) {
        uint32_t shift = (uint32_t)init->channel * 3U;
        adc->SMPR2 &= ~(7UL << shift);
        adc->SMPR2 |= ((uint32_t)init->sample_time & 7U) << shift;
    } else {
        uint32_t shift = ((uint32_t)init->channel - 10U) * 3U;
        adc->SMPR1 &= ~(7UL << shift);
        adc->SMPR1 |= ((uint32_t)init->sample_time & 7U) << shift;
    }

    /* One regular conversion in the sequence. */
    adc->SQR1 &= ~ADC_SQR1_L;
    adc->SQR3 &= ~ADC_SQR3_SQ1;
    adc->SQR3 |= (uint32_t)init->channel;
}

void mach32_adc_start(ADC_TypeDef *adc)
{
    if (adc != 0) {
        adc->CR2 |= ADC_CR2_ADON;
        adc->CR2 |= ADC_CR2_SWSTART;
    }
}

uint16_t mach32_adc_read(ADC_TypeDef *adc)
{
    if (adc == 0) {
        return 0U;
    }

    while ((adc->SR & ADC_SR_EOC) == 0U) {
        __NOP();
    }

    return (uint16_t)adc->DR;
}
