/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "gpio.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
#include "HD44780.h"
#include "myDelay.h"
#include <stdbool.h>
#include <stdio.h>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/
// Avg_Slope 4.0 	4.3 	4.6 mV/°C
// V25  		1.34 	1.43 	1.52 V
// float Temperature = 0.0;
// float Pomiar = 0.0;
const float Kp = 1.5;
const float Ki = 0.06;
const float Kd = 0.9;

const float HeatingCoefficientA = 0.1476835343;
const float HeatingCoefficientB = 19.7004294;

volatile bool TriggerPid = false;
volatile int16_t PWM = 0;
volatile int16_t CurrentTemperature = 0;
volatile int16_t TargetTemperature = 0;

void DisplayTargetTemperature(uint16_t pwm)
{
   char tab[4] =
      { 0x20, 0x20, 0x20, 0x20 };
   sprintf (tab, "%3d", pwm);
   lcd_str_XY (2, 0, tab);
}
void DisplayCurrentTemperature()
{
   if (aADCxConvertedData[0] == 4095)
   {
      return;
   }
   else
   {
      lcd_str_XY (9, 0, "HEATING");
   }

   float heaterInV = heaterInV = 660 * aADCxConvertedData[0] / 819;

   // wzmacniacz nieodwracajacy
   // Uin=(ADC*R1)/(R1+R2); // wzmacniacz operacyjny
   // R1=(Uq*R)/(Ui-Uq); //dzielnik napiecia
   // Current_Temperature=Heating_A_coefficient*R1+Heating_B_coefficient;

   float voltage2Temp = HeatingCoefficientA * heaterInV + HeatingCoefficientB;
   CurrentTemperature = (uint16_t) voltage2Temp;

   char tab[4] =
      { 0x20, 0x20, 0x20, 0x20 };
   sprintf (tab, "%3d", CurrentTemperature);
   lcd_str_XY (2, 1, tab);
}
void DsiplayCurrentPWM(uint16_t valuePWM)
{
   uint8_t result = 0;
   char tab[4] =
      { 0x20, 0x20, 0x20, 0x20 };
   result = (uint8_t) (0.2 * valuePWM);

   sprintf (tab, "%3d", result);
   lcd_str_XY (12, 1, tab);
}
void PID(int32_t* lastError, int32_t* integral, int32_t* derivative)
{
   int32_t error = 0;
   if (TargetTemperature != 0)
   {
      error = TargetTemperature - CurrentTemperature;
      *integral += error;
      *derivative = error - *lastError;

      if (*integral > 500)
         *integral = 500;
      if (*integral < 0)
         *integral = 0;

   }
   else
   {
      error = 0;
      *integral = 0;
      *derivative = 0;
   }
   *lastError = error;

   PWM = (int16_t) (Kp * error) + (Ki * *integral) + (Kd * *derivative);
   if (PWM > 500)
   {
      PWM = 500;
   }
   if (PWM < 0)
   {
      PWM = 0;
   }
   if (CurrentTemperature >= 495)
   {
      PWM = 0;
   }

   LL_TIM_OC_SetCompareCH1 (TIM2, PWM);
   DsiplayCurrentPWM (PWM);
}

int main(void)
{
   LL_APB2_GRP1_EnableClock (LL_APB2_GRP1_PERIPH_AFIO);
   LL_APB1_GRP1_EnableClock (LL_APB1_GRP1_PERIPH_PWR);

   NVIC_SetPriorityGrouping (NVIC_PRIORITYGROUP_4);

   LL_GPIO_AF_Remap_SWJ_NOJTAG ();

/* Configure the system clock */
   SystemClock_Config ();

   LL_SYSTICK_EnableIT ();

   GPIO_Init ();
   TIM1_Encoder_Init ();
   TIM2_PWM_Init ();
   TIM3_Init ();
   ADC_CONVERTED_DATA_BUFFER_SIZE = 3; // zmieniono
   ADC1_Init ();

   LL_TIM_EnableCounter (TIM1);
   LL_TIM_EnableCounter (TIM3);

   LCD_Init ();
   lcd_str_XY (0, 0, "T 000");/* Target temperature. */
   lcd_char_XY (1, 0, 0x01);
   lcd_char_XY (5, 0, 0xDF);
   lcd_char_XY (6, 0, 'C');
   lcd_str_XY (9, 0, "NOT CON");
   lcd_str_XY (0, 1, "T 000");/* Current temperature. */
   lcd_char_XY (1, 1, 0x02);
   lcd_char_XY (5, 1, 0xDF);
   lcd_char_XY (6, 1, 'C');
   lcd_char_XY (10, 1, 0x00);
   lcd_str_XY (14, 1, "0%");

   volatile uint32_t counterTim = 0;
   volatile uint32_t oldCounterTim = 0;

   int32_t lastError = 0;
   int32_t integral = 0;
   int32_t derivative = 0;

   while (1)
   {

      if ((LL_GPIO_IsInputPinSet (ENCODER_SW_GPIO_Port, ENCODER_SW_Pin)) == 1)
      {
         LL_GPIO_ResetOutputPin (Buzzer_GPIO_Port, Buzzer_Pin);
      }
      else
      {
         LL_GPIO_SetOutputPin (Buzzer_GPIO_Port, Buzzer_Pin);
      }

      counterTim = LL_TIM_GetCounter (TIM1);
      if (counterTim != oldCounterTim)
      {
         oldCounterTim = counterTim;
         if ((LL_TIM_GetDirection (TIM1) == LL_TIM_COUNTERDIRECTION_UP) && (counterTim == 0))
         {
            LL_TIM_SetCounter (TIM1, 50); // set max.
            counterTim = 500;
         }
         else if ((LL_TIM_GetDirection (TIM1) == LL_TIM_COUNTERDIRECTION_DOWN) && (counterTim == 50))
         {
            LL_TIM_SetCounter (TIM1, 0);
            counterTim = 10;
         }
         else
         {
            counterTim = 10 * counterTim;
         }

         TargetTemperature = counterTim;
         DisplayTargetTemperature (counterTim);
      }

      if (TriggerPid == true)
      {
         PID (&lastError, &integral, &derivative);
         DisplayCurrentTemperature ();
         TriggerPid = false;
      }

      // aADCxConvertedData[2]=123;
      // aADCxConvertedData[3]=0;

      // Pomiar = (3300*aADCxConvertedData[2])/4095;
      // uhADCxConvertedData_Voltage_mVolt = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI,
      // aADCxConvertedData[2], LL_ADC_RESOLUTION_12B); Temperature = ((1.45 -
      // uhADCxConvertedData_Voltage_mVolt) / 4.3) + 25;
   }
/* USER CODE END 3 */
}

void SystemClock_Config(void)
{
   LL_FLASH_SetLatency (LL_FLASH_LATENCY_2);

   if (LL_FLASH_GetLatency () != LL_FLASH_LATENCY_2)
   {
      Error_Handler ();
   }
   LL_RCC_HSE_Enable ();

/* Wait till HSE is ready */
   while (LL_RCC_HSE_IsReady () != 1)
   {
   }
   LL_RCC_PLL_ConfigDomain_SYS (LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
   LL_RCC_PLL_Enable ();

/* Wait till PLL is ready */
   while (LL_RCC_PLL_IsReady () != 1)
   {
   }
   LL_RCC_SetAHBPrescaler (LL_RCC_SYSCLK_DIV_1);
   LL_RCC_SetAPB1Prescaler (LL_RCC_APB1_DIV_2);
   LL_RCC_SetAPB2Prescaler (LL_RCC_APB2_DIV_1);
   LL_RCC_SetSysClkSource (LL_RCC_SYS_CLKSOURCE_PLL);

/* Wait till System clock is ready */
   while (LL_RCC_GetSysClkSource () != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
   {
   }
   LL_Init1msTick (72000000);
   LL_SYSTICK_SetClkSource (LL_SYSTICK_CLKSOURCE_HCLK);
   LL_SetSystemCoreClock (72000000);
   LL_RCC_SetADCClockSource (LL_RCC_ADC_CLKSRC_PCLK2_DIV_6);
}

void Error_Handler(void)
{
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
