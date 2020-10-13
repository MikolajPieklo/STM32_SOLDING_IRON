#include "tim.h"

void TIM1_Encoder_Init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    // PA8   ------> TIM1_CH1
    // PA9   ------> TIM1_CH2

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_FLOATING);

    LL_TIM_SetEncoderMode(TIM1, LL_TIM_ENCODERMODE_X2_TI1);
    LL_TIM_IC_SetActiveInput(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV32_N8);
    LL_TIM_IC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetActiveInput(TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV32_N8);
    LL_TIM_IC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);

    LL_TIM_SetPrescaler(TIM1, 0);
    LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_DOWN);
    LL_TIM_SetAutoReload(TIM1, 50);
    LL_TIM_SetClockDivision(TIM1, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetRepetitionCounter(TIM1, 0);

    LL_TIM_DisableARRPreload(TIM1);
    LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM1);
}

void TIM2_PWM_Init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA); /* Enable the peripheral clock of GPIOs */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);  /* Enable the timer peripheral clock */

    // PA15     ------> TIM2_CH1
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_15, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_15, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_15, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_AF_RemapPartial1_TIM2();

    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);

    // LL_TIM_SetClockDivision       (TIM2, LL_TIM_CLOCKDIVISION_DIV2);
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP); /* Set counter mode */
    LL_TIM_SetPrescaler(
        TIM2, 599); /* Set the pre-scaler value to have TIM2 counter clock equal to 10 kHz */
    LL_TIM_EnableARRPreload(TIM2);
    LL_TIM_SetAutoReload(TIM2,
                         500); /* Set the auto-reload value to have a counter frequency of 100 Hz */

    /*********************************/
    /* Output waveform configuration */
    /*********************************/
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1); /* Set output mode */
    LL_TIM_OC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1,
                          LL_TIM_OCPOLARITY_HIGH); /* Set output channel polarity */
    LL_TIM_OC_SetCompareCH1(TIM2, 1);              /* Wypelnienie*/
    LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH1);

    /**************************/
    /* TIM2 interrupts set-up */
    /**************************/
    LL_TIM_EnableIT_CC1(TIM2); /* Enable the capture/compare interrupt for channel 1*/
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1); /* Enable output channel 1 */
    LL_TIM_EnableCounter(TIM2);                        /* Enable counter */
    LL_TIM_GenerateEvent_UPDATE(TIM2);                 /* Force update generation */
}

void TIM3_Init(void)
{
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3); /* Enable the timer peripheral clock */

    NVIC_SetPriority(TIM3_IRQn, 0);
    NVIC_EnableIRQ(TIM3_IRQn);

    LL_TIM_SetCounterMode(TIM3, LL_TIM_COUNTERMODE_UP); /* Set counter mode */
    LL_TIM_SetPrescaler(TIM3, 7199);
    LL_TIM_EnableARRPreload(TIM3);
    LL_TIM_SetAutoReload(TIM3, 9999);
    LL_TIM_SetClockDivision(TIM3, LL_TIM_CLOCKDIVISION_DIV1);

    // LL_TIM_OC_SetCompareCH1(TIM3, 9999);
    // LL_TIM_EnableIT_CC1(TIM3);
    // LL_TIM_GenerateEvent_CC1(TIM3);

    LL_TIM_EnableIT_UPDATE(TIM3);
    LL_TIM_GenerateEvent_UPDATE(TIM3); /* Force update generation */

    LL_TIM_EnableCounter(TIM3); /* Enable counter */
}
