/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_it.h"
#include "main.h"
#include <stdbool.h>

extern uint32_t SysTickValue;
extern bool TriggerPid;

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
        LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        TriggerPid = true;
    }
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
}*/
