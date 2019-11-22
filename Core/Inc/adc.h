#ifndef __adc_H
#define __adc_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

 uint32_t ADC_CONVERTED_DATA_BUFFER_SIZE;
 uint16_t aADCxConvertedData[4];
 uint32_t wait_loop_index;

 #define ADC_DELAY_ENABLE_CALIB_CPU_CYCLES  (LL_ADC_DELAY_ENABLE_CALIB_ADC_CYCLES * 32)

void ADC1_Init(void);


#ifdef __cplusplus
}
#endif
#endif /*__ adc_H */
