#include <wdg.h>

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_wwdg.h>
#include <stm32f1xx_ll_rcc.h>

void WDG_Init(void)
{
   /* Enable the peripheral clock of DBG register (uncomment for debug purpose) */
   /*LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_WWDG_STOP); */

   /* Enable the peripheral clock WWDG */
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);

   /* Configure WWDG */
   /* (1) set prescaler to have a rollover each about ~2s */
   /* (2) set window value to same value (~2s) as downcounter in order to ba able to refresh the WWDG almost immediately */
   /* (3) Refresh WWDG before activate it */
   /* (4) Activate WWDG */
   LL_WWDG_SetPrescaler(WWDG, LL_WWDG_PRESCALER_8); /* (1) */
   //LL_WWDG_SetWindow(WWDG,0x7E);                  /* (2) */
   LL_WWDG_SetCounter(WWDG, 0X7E);                  /* (3) */
   LL_WWDG_Enable(WWDG);                            /* (4) */
}

void WDG_Reset(void)
{
   if (LL_RCC_IsActiveFlag_WWDGRST())
   {
      LL_RCC_ClearResetFlags();    /* clear WWDG reset flag */
   }
}

void WDG_Service(void)
{
   LL_WWDG_SetCounter(WWDG, 0X7E);
}
