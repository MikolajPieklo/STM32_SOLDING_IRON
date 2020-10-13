/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_it.h"
#include "main.h"

extern uint32_t SysTickValue;
extern uint8_t PID_Enable;
extern uint8_t UpgradeDisplay;

void NMI_Handler(void)
{
}


void HardFault_Handler(void)
{
    while (1) {
    }
}

void MemManage_Handler(void)
{
    while (1) {
    }
}

void BusFault_Handler(void)
{
    while (1) {
    }
}

void UsageFault_Handler(void)
{
    while (1) {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    SysTickValue++;
}

void TIM2_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_CC1(TIM2) == 1) /* Check whether CC1 interrupt is pending */
    {
        LL_TIM_ClearFlag_CC1(TIM2); /* Clear the update interrupt flag*/
    }
}

void TIM3_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM3) == 1) {
        LL_TIM_ClearFlag_UPDATE(TIM3);
        UpgradeDisplay = 1;
        LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        PID_Enable = 1;
    }

    /*if(LL_TIM_IsActiveFlag_CC1(TIM3) == 1)
    {
        LL_TIM_ClearFlag_CC1(TIM3);
        LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }*/
}

void DMA1_Channel1_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC1(DMA1) == 1) {
        LL_DMA_ClearFlag_TC1(DMA1);
    }
    if (LL_DMA_IsActiveFlag_GI1(DMA1) == 1) {
        LL_DMA_ClearFlag_GI1(DMA1);
    }
    if (LL_DMA_IsActiveFlag_TE1(DMA1) == 1) {
        LL_DMA_ClearFlag_TE1(DMA1);
    }
}

/*void ADC1_2_IRQHandler(void)
{
    volatile int a;
    a=0;
    if(LL_ADC_IsActiveFlag_EOS(ADC1) != 0)
    {
        LL_ADC_ClearFlag_EOS(ADC1);
        a = LL_ADC_REG_ReadConversionData12(ADC1);
        a=0;
    }
}*/
