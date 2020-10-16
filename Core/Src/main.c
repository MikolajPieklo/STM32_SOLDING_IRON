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
#include <stdlib.h>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/
// Avg_Slope 4.0 	4.3 	4.6 mV/°C
// V25  		1.34 	1.43 	1.52 V
// float Temperature = 0.0;
// float Pomiar = 0.0;

const float kp = 1.5;
const float ki = 0.06;
const float kd = 0.9;

const float Heating_A_coefficient = 0.1476835343;
const float Heating_B_coefficient = 19.7004294;

volatile bool TriggerPid = false;

volatile int16_t PWM = 0;

volatile int16_t Current_Temperature = 0;
volatile int16_t Target_Temperature = 0;

void DisplayTargetTemperature(uint16_t PWM)
{
    char tab[4] = {0x20, 0x20, 0x20, 0x20};
    if (PWM >= 100) {
        itoa(PWM, tab, 10);
        lcd_str_XY(2, 0, tab);
    } else if (PWM >= 10 && PWM < 100) {
        itoa(PWM, tab + 1, 10);
        lcd_str_XY(2, 0, tab);
    } else {
        itoa(PWM, tab + 2, 10);
        lcd_str_XY(2, 0, tab);
    }
}
void DisplayCurrentTemperature()
{
    char tab[4] = {0x20, 0x20, 0x20, 0x20};
    float Heater_V = 0.0;
    float U2Temp = 0.0;

    Heater_V = 660 * aADCxConvertedData[0] / 819;

    // wzmacniacz nieodwracajacy
    // Uin=(ADC*R1)/(R1+R2); // wzmacniacz operacyjny
    // R1=(Uq*R)/(Ui-Uq); //dzielnik napiecia
    // Current_Temperature=Heating_A_coefficient*R1+Heating_B_coefficient;

    U2Temp = Heating_A_coefficient * Heater_V + Heating_B_coefficient;
    Current_Temperature = (uint16_t)U2Temp;

    itoa(Current_Temperature, tab, 10);
    if (Current_Temperature < 100) {
        lcd_str_XY(2, 1, " ");
        lcd_str_XY(3, 1, tab);
    } else
        lcd_str_XY(2, 1, tab);
}
void DsiplayCurrentPWM(uint16_t valuePWM)
{
    uint8_t Result = 0;
    char tab[4] = {0x20, 0x20, 0x20, 0x20};

    Result = 0.2 * valuePWM;

    itoa(Result, tab, 10);

    if (Result == 100) // 100
    {
        lcd_str_XY(12, 1, tab);
    } else if (Result > 9 && Result < 100) // <10; 99>
    {
        lcd_str_XY(12, 1, " ");
        lcd_str_XY(13, 1, tab);
    } else // <0; 9>
    {
        lcd_str_XY(12, 1, "  ");
        lcd_str_XY(14, 1, tab);
    }
}
void PID(int32_t *Last_Error, int32_t *Integral, int32_t *Derivative)
{
    int32_t Error = 0;
    if (Target_Temperature != 0) {
        Error = Target_Temperature - Current_Temperature;
        Integral += Error;
        *Derivative = Error - *Last_Error;

        if (*Integral > 500)
            *Integral = 500;
        if (*Integral < 0)
            *Integral = 0;

    } else {
        Error = 0;
        *Integral = 0;
        *Derivative = 0;
    }
    *Last_Error = Error;

    PWM = (int16_t)(kp * Error) + (ki * *Integral) + (kd * *Derivative);
    if (PWM > 500) {
        PWM = 500;
    }
    if (PWM < 0) {
        PWM = 0;
    }
    if (Current_Temperature >= 495) {
        PWM = 0;
    }

    LL_TIM_OC_SetCompareCH1(TIM2, PWM);
    DsiplayCurrentPWM(PWM);
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
    TIM3_Init();
    ADC_CONVERTED_DATA_BUFFER_SIZE = 3; // zmieniono
    ADC1_Init();

    LL_TIM_EnableCounter(TIM1);
    LL_TIM_EnableCounter(TIM3);

    LCD_Init();
    lcd_str_XY(0, 0, "T 000"); // Target
    lcd_char_XY(1, 0, 0x01);
    lcd_char_XY(5, 0, 0xDF);
    lcd_char_XY(6, 0, 'C');
    lcd_str_XY(9, 0, "HEATING");
    lcd_str_XY(0, 1, "T 000"); // Current
    lcd_char_XY(1, 1, 0x02);
    lcd_char_XY(5, 1, 0xDF);
    lcd_char_XY(6, 1, 'C');
    lcd_char_XY(10, 1, 0x00);
    lcd_str_XY(14, 1, "0%");

    volatile uint32_t CounterTIM = 0;
    volatile uint32_t OLDCounterTIM = 0;

    int32_t Last_Error = 0;
    int32_t Integral = 0;
    int32_t Derivative = 0;

    while (1) {

        if ((LL_GPIO_IsInputPinSet(ENCODER_SW_GPIO_Port, ENCODER_SW_Pin)) == 1) {
            LL_GPIO_ResetOutputPin(Buzzer_GPIO_Port, Buzzer_Pin);
        } else {
            LL_GPIO_SetOutputPin(Buzzer_GPIO_Port, Buzzer_Pin);
        }
        CounterTIM = LL_TIM_GetCounter(TIM1);

        if (CounterTIM != OLDCounterTIM) {
            OLDCounterTIM = CounterTIM;
            if ((LL_TIM_GetDirection(TIM1) == LL_TIM_COUNTERDIRECTION_UP) && (CounterTIM == 0)) {
                LL_TIM_SetCounter(TIM1, 50);
                CounterTIM = 500;
            } else if ((LL_TIM_GetDirection(TIM1) == LL_TIM_COUNTERDIRECTION_DOWN)
                       && (CounterTIM == 50)) {
                LL_TIM_SetCounter(TIM1, 0);
                CounterTIM = 10;
            } else {
                CounterTIM = 10 * CounterTIM;
            }

            Target_Temperature = CounterTIM;
            DisplayTargetTemperature(CounterTIM);
        }

        if (TriggerPid == true) {
            PID(&Last_Error, &Integral, &Derivative);
            DisplayCurrentTemperature();
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
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2) {
        Error_Handler();
    }
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while (LL_RCC_HSE_IsReady() != 1) {
    }
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1) {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
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
