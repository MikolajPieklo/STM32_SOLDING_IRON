/*
 * myDelay.c
 *
 *  Created on: 08.03.2019
 *      Author: Mikolaj
 */
#include "myDelay.h"

void Delay(uint32_t msDelay)
{
    tickstart = SysTickValue;
    while ((SysTickValue - tickstart) < msDelay) {
    }
}
