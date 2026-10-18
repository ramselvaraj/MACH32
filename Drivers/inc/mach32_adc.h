/* MACH32 analog-to-digital converter driver. */

#ifndef MACH32_ADC_H
#define MACH32_ADC_H

#include <stdint.h>

#include "stm32f4xx.h"

typedef enum {
    MACH32_ADC_RESOLUTION_12_BIT,
    MACH32_ADC_RESOLUTION_10_BIT,
    MACH32_ADC_RESOLUTION_8_BIT,
    MACH32_ADC_RESOLUTION_6_BIT,
} mach32_adc_resolution_t;

typedef enum {
    MACH32_ADC_SAMPLE_3_CYCLES,
    MACH32_ADC_SAMPLE_15_CYCLES,
    MACH32_ADC_SAMPLE_28_CYCLES,
    MACH32_ADC_SAMPLE_56_CYCLES,
    MACH32_ADC_SAMPLE_84_CYCLES,
    MACH32_ADC_SAMPLE_112_CYCLES,
    MACH32_ADC_SAMPLE_144_CYCLES,
    MACH32_ADC_SAMPLE_480_CYCLES,
} mach32_adc_sample_time_t;

typedef struct {
    ADC_TypeDef *instance;
    uint8_t channel;
    mach32_adc_resolution_t resolution;
    mach32_adc_sample_time_t sample_time;
    uint8_t continuous;
} mach32_adc_init_t;

void mach32_adc_init(const mach32_adc_init_t *init);
void mach32_adc_start(ADC_TypeDef *adc);
uint16_t mach32_adc_read(ADC_TypeDef *adc);

#endif /* MACH32_ADC_H */
