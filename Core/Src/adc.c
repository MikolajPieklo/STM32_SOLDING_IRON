/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"

void ADC1_Init() {
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1); // Enable the peripheral clock of DMA

	NVIC_SetPriority(DMA1_Channel1_IRQn, 1); // DMA IRQ lower priority than ADC IRQ
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	LL_DMA_SetDataTransferDirection	(DMA1, LL_DMA_CHANNEL_1,	LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetChannelPriorityLevel	(DMA1, LL_DMA_CHANNEL_1,	LL_DMA_PRIORITY_HIGH);

	/*LL_DMA_SetMode			(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);
	LL_DMA_SetPeriphIncMode	(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode	(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize	(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);
	LL_DMA_SetMemorySize	(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

	LL_DMA_ConfigAddresses	(DMA1, LL_DMA_CHANNEL_1, LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
			(uint32_t)&aADCxConvertedData,LL_DMA_DIRECTION_PERIPH_TO_MEMORY); // Set DMA transfer addresses of source and destination
*/

	  LL_DMA_ConfigTransfer(DMA1,LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_MODE_CIRCULAR  |
	                        LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_HALFWORD |
	                        LL_DMA_MDATAALIGN_HALFWORD | LL_DMA_PRIORITY_HIGH);

	  LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_1,LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),(uint32_t)&aADCxConvertedData,
	                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_SetDataLength	(DMA1, LL_DMA_CHANNEL_1,ADC_CONVERTED_DATA_BUFFER_SIZE); // Set DMA transfer size
	LL_DMA_EnableIT_TC		(DMA1, LL_DMA_CHANNEL_1);		// transfer complete
	//LL_DMA_EnableIT_HT		(DMA1, LL_DMA_CHANNEL_1); 	// half transfer
	LL_DMA_EnableIT_TE		(DMA1, LL_DMA_CHANNEL_1); 		// transfer error
	LL_DMA_EnableChannel	(DMA1, LL_DMA_CHANNEL_1); 		// Enable the DMA transfer


	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA); 			/* Enable GPIO Clock*/
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG); 	/* Configure GPIO in analog mode to be used as ADC input*/

	NVIC_SetPriority(ADC1_IRQn, 0); /* ADC IRQ greater priority than DMA IRQ */
	NVIC_EnableIRQ	(ADC1_IRQn);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1); 	/* Enable ADC clock (core clock) */
	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
			(LL_ADC_PATH_INTERNAL_VREFINT | LL_ADC_PATH_INTERNAL_TEMPSENSOR)); /* Set ADC measurement path to internal channels */

	/*## Configuration of ADC hierarchical scope: multimode ####################*/
	//LL_ADC_SetMultimode              (__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_INDEPENDENT);         /* Set ADC multimode configuration */
	//LL_ADC_SetMultiDMATransfer       (__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_REG_DMA_EACH_ADC);    /* Set ADC multimode DMA transfer */
	//LL_ADC_SetMultiTwoSamplingDelay  (__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE); /* Set ADC multimode: delay between 2 sampling phases */

	//LL_ADC_SetResolution			(ADC1, LL_ADC_DATA_ALIGN_RIGHT); 			/* Set ADC conversion data alignment */
	LL_ADC_SetSequencersScanMode	(ADC1, LL_ADC_SEQ_SCAN_ENABLE); 			/* Set ADC sequencers scan mode, for all ADC groups*/
	LL_ADC_REG_SetTriggerSource		(ADC1, LL_ADC_REG_TRIG_SOFTWARE); 			/* Set ADC group regular trigger source */
	LL_ADC_REG_SetContinuousMode	(ADC1, LL_ADC_REG_CONV_CONTINUOUS); 		/* Set ADC group regular continuous mode */
	LL_ADC_REG_SetDMATransfer		(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED); 	/* Set ADC group regular conversion data transfer */
	LL_ADC_REG_SetSequencerLength	(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS); 	/* Set ADC group regular sequencer length and scan direction */

	//LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);    /* Set ADC group regular sequencer discontinuous mode */

	LL_ADC_REG_SetSequencerRanks	(ADC1, LL_ADC_REG_RANK_1, 	LL_ADC_CHANNEL_0);
	LL_ADC_REG_SetSequencerRanks	(ADC1, LL_ADC_REG_RANK_2,	LL_ADC_CHANNEL_VREFINT);
	LL_ADC_REG_SetSequencerRanks	(ADC1, LL_ADC_REG_RANK_3,	LL_ADC_CHANNEL_TEMPSENSOR);

	//LL_ADC_INJ_SetTriggerSource     (ADC1, LL_ADC_INJ_TRIG_SOFTWARE);             /* Set ADC group injected trigger source */
	//LL_ADC_INJ_SetTriggerEdge       (ADC1, LL_ADC_INJ_TRIG_EXT_RISING);           /* Set ADC group injected trigger polarity */
	//LL_ADC_INJ_SetTrigAuto          (ADC1, LL_ADC_INJ_TRIG_INDEPENDENT);          /* Set ADC group injected conversion trigger  */
	//LL_ADC_INJ_SetSequencerLength   (ADC1, LL_ADC_INJ_SEQ_SCAN_DISABLE);          /* Set ADC group injected sequencer length and scan direction */
	//LL_ADC_INJ_SetSequencerDiscont  (ADC1, LL_ADC_INJ_SEQ_DISCONT_DISABLE);       /* Set ADC group injected sequencer discontinuous mode */
	//LL_ADC_INJ_SetSequencerRanks    (ADC1, LL_ADC_INJ_RANK_1, LL_ADC_CHANNEL_0);  /* Set ADC group injected sequence: channel on the selected sequence rank. */

	LL_ADC_SetChannelSamplingTime	(ADC1, LL_ADC_CHANNEL_0,			LL_ADC_SAMPLINGTIME_239CYCLES_5);
	LL_ADC_SetChannelSamplingTime	(ADC1, LL_ADC_CHANNEL_VREFINT,		LL_ADC_SAMPLINGTIME_239CYCLES_5);
	LL_ADC_SetChannelSamplingTime	(ADC1, LL_ADC_CHANNEL_TEMPSENSOR,	LL_ADC_SAMPLINGTIME_239CYCLES_5);

	//LL_ADC_SetAnalogWDMonitChannels(ADC1, LL_ADC_AWD_DISABLE);

	/* Set ADC analog watchdog: thresholds */
	//LL_ADC_SetAnalogWDThresholds(ADC1, LL_ADC_AWD_THRESHOLD_HIGH, __LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B));
	//LL_ADC_SetAnalogWDThresholds(ADC1, LL_ADC_AWD_THRESHOLD_LOW, 0x000);

	//LL_ADC_EnableIT_EOS(ADC1);  /* Enable interruption ADC group regular end of sequence conversions */

	LL_ADC_Enable(ADC1);		/* Enable ADC */
	wait_loop_index = (ADC_DELAY_ENABLE_CALIB_CPU_CYCLES >> 1);

	while (wait_loop_index != 0)
	{
		wait_loop_index--;
	}

	//LL_ADC_StartCalibration(ADC1); // Run ADC self calibration
	//while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0);


    LL_ADC_REG_StartConversionSWStart(ADC1);

    while (LL_ADC_IsActiveFlag_EOS(ADC1) == 0)
    {
    }
    LL_ADC_ClearFlag_EOS(ADC1);

}
