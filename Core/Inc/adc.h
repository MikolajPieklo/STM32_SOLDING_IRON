#ifndef __adc_H
#define __adc_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define ADC_DELAY_ENABLE_CALIB_CPU_CYCLES (LL_ADC_DELAY_ENABLE_CALIB_ADC_CYCLES * 32)

void ADC1_Init(uint16_t* data, uint32_t size);

#ifdef __cplusplus
}
#endif
#endif /*__ adc_H */
