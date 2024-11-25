/*
 * pid.h
 *
 *  Created on: Sep 19, 2021
 *      Author: mkpk
 */

#ifndef APP_PID_H_
#define APP_PID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct Temperature
{
   int32_t current;
   int32_t target;
} temperature_t;

void PID(int32_t* pwm, temperature_t* temperature);

#ifdef __cplusplus
}
#endif

#endif /* APP_PID_H_ */
