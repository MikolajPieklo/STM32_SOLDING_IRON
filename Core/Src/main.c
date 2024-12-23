/* Includes ------------------------------------------------------------------*/
#include <adc.h>
#include <gpio.h>
#include <main.h>
#include <tim.h>
#include <wdg.h>

/* Private includes ----------------------------------------------------------*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <HD44780.h>
#include <delay.h>
#include <pid.h>
#include <test.hpp>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/
#define ADC_BUFFER_SIZE 100
#define MAX_TEMPERATURE 500

typedef enum State
{
   state_ok = 0,
   state_target_temperature_high = 1,
   state_adcfull = 2,
   state_current_temperature_high = 3
} state_t;

// Avg_Slope 4.0   4.3    4.6 mV/°C
// V25       1.34  1.43   1.52 V
// const float HeatingCoefficientA = 0.1476835343;
// const float HeatingCoefficientB = 19.7004294;

// data for adc without cal
// const float HeatingCoefficientA = 0.37386446;
// const float HeatingCoefficientB = -121.49858;

// data for adc with cal
const float HeatingCoefficientA = 0.39762748;
const float HeatingCoefficientB = -150.204373;

uint32_t ADC_CONVERTED_DATA_BUFFER_SIZE = ADC_BUFFER_SIZE;
uint16_t aADCxConvertedData[ADC_BUFFER_SIZE];

volatile bool   TriggerPid = false;
state_t         State;
static bool     beep_is_active = false;
static uint32_t beep_time = 0;
static uint32_t beep_active_time = 30;
static uint32_t key_state = 0;
static uint32_t key_time = 0;

static inline void displayTargetTemperature(int32_t temperature);
static inline void displayCurrentTemperature(temperature_t *temperature);
static inline void dsiplayCurrentPWM(uint32_t valuePWM);
static inline void checkError(void);
static void        beep_start(uint32_t ms);
static void        beep_service(void);
static void        key_machine(void);

static inline void displayTargetTemperature(int32_t temperature)
{
   char *tab = (char *) malloc(sizeof(char *) * 4);

   if (temperature > 999)
   {
      sprintf(tab, "E01", temperature);
      LCD_Str_XY(2, 0, tab);
      State = state_target_temperature_high;
   }
   else
   {
      sprintf(tab, "%3d", temperature);
      LCD_Str_XY(2, 0, tab);
   }
   free(tab);
}

static inline void displayCurrentTemperature(temperature_t *temperature)
{
   do
   {
      uint32_t i, sum = 0;

      if (aADCxConvertedData[0] == 4095)
      {
         LCD_Str_XY(9, 0, "ADCFULL");
         State = state_adcfull;
         break;
      }
      LCD_Str_XY(9, 0, "HEATING");

      for (i = 0; i < ADC_BUFFER_SIZE; i++)
      {
         sum += aADCxConvertedData[i];
      }

      sum = (uint32_t) sum / ADC_BUFFER_SIZE;

      float heaterInV = 660 * sum / 819;

      // wzmacniacz nieodwracajacy
      // Uin=(ADC*R1)/(R1+R2); // wzmacniacz operacyjny
      // R1=(Uq*R)/(Ui-Uq); //dzielnik napiecia
      // Current_Temperature=Heating_A_coefficient*R1+Heating_B_coefficient;

      float voltage2Temp = HeatingCoefficientA * heaterInV + HeatingCoefficientB;
      temperature->current = (int16_t) voltage2Temp;

      if (temperature->current > MAX_TEMPERATURE)
      {
         State = state_current_temperature_high;
         break;
      }

      char *tab = (char *) malloc(sizeof(char *) * 4);
      sprintf(tab, "%3d", temperature->current);
      LCD_Str_XY(2, 1, tab);
      free(tab);
   } while (0);
}

static inline void dsiplayCurrentPWM(uint32_t valuePWM)
{
   uint8_t result = 0;
   char   *tab = (char *) malloc(sizeof(char *) * 4);
   if (valuePWM > 999)
   {
      sprintf(tab, "E02", result);
      LCD_Str_XY(12, 1, tab);
   }
   else
   {
      result = (uint8_t) (0.2 * valuePWM);
      sprintf(tab, "%3d", result);
      LCD_Str_XY(12, 1, tab);
   }
   free(tab);
}

static inline void checkError(void)
{
   static bool printed = false;
   if (State != state_ok)
   {
      if (printed == false)
      {
         switch (State)
         {
         case state_target_temperature_high:
            LCD_Str_XY(8, 1, "E1");
            break;
         case state_adcfull:
            LCD_Str_XY(8, 1, "E2");
            break;
         case state_current_temperature_high:
            LCD_Str_XY(8, 1, "E3");
            break;
         default:
            LCD_Str_XY(8, 1, "E*");
            break;
         }
         printed = true;
      }
      else
      {
         LCD_Str_XY(8, 1, "  ");
         printed = false;
      }
   }
}

static void beep_start(uint32_t ms)
{
   if (false == beep_is_active)
   {
      beep_time = TS_Get_ms();
      beep_is_active = true;
      beep_active_time = ms;
   }
}

static void beep_service(void)
{
   if (true == beep_is_active)
   {
      if (beep_time + beep_active_time > TS_Get_ms())
      {
         LL_GPIO_SetOutputPin(Buzzer_GPIO_Port, Buzzer_Pin);
      }
      else
      {
         LL_GPIO_ResetOutputPin(Buzzer_GPIO_Port, Buzzer_Pin);
         beep_is_active = false;
      }
   }
}

static void key_machine(void)
{
   if (LL_GPIO_IsInputPinSet(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin) == 0)
   {
      switch (key_state)
      {
      case 0:
         key_time = TS_Get_ms();
         key_state = 1;
         break;
      case 1:
         if (key_time + 1000 < TS_Get_ms())
         {
            key_state = 2;
         }
         break;
      case 2:
         break;
      }
   }
   else
   {
      if (key_state == 1)
      {
         LL_TIM_SetCounter(TIM1, 32);
         key_state = 0;
         beep_start(30);
      }
      else if (key_state == 2)
      {
         LL_TIM_SetCounter(TIM1, 10);
         key_state = 0;
         beep_start(200);
      }
      else
      {
      }
   }
}

int main(void)
{
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

   NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

   LL_GPIO_AF_Remap_SWJ_NOJTAG();

   /* Configure the system clock */
   SystemClock_Config();

   LL_SYSTICK_EnableIT();

   GPIO_Init();
   TIM1_Encoder_Init();
   TIM2_PWM_Init();
   LCD_Init();
   ADC1_Init(aADCxConvertedData, ADC_CONVERTED_DATA_BUFFER_SIZE);
   WDG_Init();
   TIM3_Init();

   LL_TIM_EnableCounter(TIM1);
   LL_TIM_EnableCounter(TIM3);

   temperature_t temperature = {0, 0};
   int32_t       pwm = 0;

   uint32_t counterTim = 0;
   uint32_t oldCounterTim = 0;

   beep_start(30);

   while (1)
   {
      beep_service();
      key_machine();

      counterTim = LL_TIM_GetCounter(TIM1);
      if (counterTim != oldCounterTim)
      {
         if ((LL_TIM_GetDirection(TIM1) == LL_TIM_COUNTERDIRECTION_UP) && (oldCounterTim >= 40)
             && (counterTim <= 10))
         {
            LL_TIM_SetCounter(TIM1, 50); // set max.
            counterTim = 50;
         }
         else if ((LL_TIM_GetDirection(TIM1) == LL_TIM_COUNTERDIRECTION_DOWN)
                  && (oldCounterTim <= 10) && (counterTim >= 40))
         {
            LL_TIM_SetCounter(TIM1, 0);
            counterTim = 0;
         }
         oldCounterTim = counterTim;
         temperature.target = (int32_t) 10 * counterTim;
         displayTargetTemperature(temperature.target);
      }

      if (TriggerPid == true)
      {
         displayCurrentTemperature(&temperature);

         PID(&pwm, &temperature);

         if (State == state_ok)
         {
            dsiplayCurrentPWM(pwm);
            LL_TIM_OC_SetCompareCH1(TIM2, pwm);
         }
         else
         {
            dsiplayCurrentPWM(0);
            LL_TIM_OC_SetCompareCH1(TIM2, 0);
         }

         TriggerPid = false;
         checkError();
      }
      WDG_Service();

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
   LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

   if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
   {
      Error_Handler();
   }
   LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
   while (LL_RCC_HSE_IsReady() != 1)
   {
   }
   LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
   LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
   while (LL_RCC_PLL_IsReady() != 1)
   {
   }
   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
   LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
   LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
   LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
   while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
   {
   }
   LL_Init1msTick(72000000);
   LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
   LL_SetSystemCoreClock(72000000);
   LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_6);
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
