/* Includes ------------------------------------------------------------------*/
#include <main.h>
#include <adc.h>
#include <gpio.h>
#include <tim.h>

/* Private includes ----------------------------------------------------------*/
#include <HD44780.h>
#include <myDelay.h>
#include <pid.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/
// Avg_Slope 4.0   4.3    4.6 mV/°C
// V25       1.34  1.43   1.52 V

const float HeatingCoefficientA = 0.1476835343;
const float HeatingCoefficientB = 19.7004294;

extern uint32_t ADC_CONVERTED_DATA_BUFFER_SIZE;
extern uint16_t aADCxConvertedData[4];

volatile bool TriggerPid = false;

static inline void displayTargetTemperature(uint32_t temperature);
static inline void displayCurrentTemperature(Temperature_t* temperature);
static inline void dsiplayCurrentPWM(uint32_t valuePWM);

static inline void displayTargetTemperature(uint32_t temperature)
{
   char* tab = (char*) malloc (sizeof(char*) * 4);

   if (temperature > 999)
   {
      sprintf (tab, "E01", temperature);
      lcd_str_XY (2, 0, tab);
   }
   else
   {
      sprintf (tab, "%3d", temperature);
      lcd_str_XY (2, 0, tab);
   }
   free (tab);
}

static inline void displayCurrentTemperature(Temperature_t* temperature)
{
   if (aADCxConvertedData[0] == 4095)
   {
      lcd_str_XY (9, 0, "ADCFULL");
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
   temperature->current = (uint16_t) voltage2Temp;

   char* tab = (char*) malloc (sizeof(char*) * 4);
   sprintf (tab, "%3d", temperature->current);
   lcd_str_XY (2, 1, tab);
   free (tab);
}

static inline void dsiplayCurrentPWM(uint32_t valuePWM)
{
   uint8_t result = 0;
   char* tab = (char*) malloc (sizeof(char*) * 4);
   if (valuePWM > 999)
   {
      sprintf (tab, "E02", result);
      lcd_str_XY (12, 1, tab);
   }
   else
   {
      result = (uint8_t) (0.2 * valuePWM);
      sprintf (tab, "%3d", result);
      lcd_str_XY (12, 1, tab);
   }
   free (tab);
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
   LCD_Init ();

   ADC_CONVERTED_DATA_BUFFER_SIZE = 3; // zmieniono
   ADC1_Init ();

   LL_TIM_EnableCounter (TIM1);
   LL_TIM_EnableCounter (TIM3);

   Temperature_t temperature = {0, 0};
   uint32_t pwm = 0;

   uint32_t counterTim = 0;
   uint32_t oldCounterTim = 0;

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
         temperature.target = counterTim;

         displayTargetTemperature(temperature.target);
      }

      if (TriggerPid == true)
      {
         displayCurrentTemperature(&temperature);

         PID (&pwm, &temperature);

         LL_TIM_OC_SetCompareCH1(TIM2, pwm);
         dsiplayCurrentPWM(pwm);

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
