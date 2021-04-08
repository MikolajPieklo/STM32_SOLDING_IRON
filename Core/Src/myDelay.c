/*
 * myDelay.c
 *
 *  Created on: 08.03.2019
 *      Author: Mikolaj
 */
#include "myDelay.h"

volatile uint32_t SysTickValue;
volatile uint32_t tickstart;

void Delay(uint32_t msDelay)
{
    tickstart = SysTickValue;
    while ((SysTickValue - tickstart) < msDelay) {
    }
}
