/*
 * pid.c
 *
 *  Created on: Sep 19, 2021
 *      Author: mkpk
 */

#include "pid.h"

const float Kp = 1.8;
const float Ki = 0.9;
const float Kd = 0.1;

static int32_t lastError = 0;
static int32_t integral = 0;
static int32_t derivative = 0;

void PID(int32_t* pwm, temperature_t* temperature)
{
   int32_t error = 0;

   if (temperature->target != 0)
   {
      error = temperature->target - temperature->current;
      integral += error;
      derivative = error - lastError;

      if (integral > 500)
      {
         integral = 500;
      }
      if (integral < -500)
      {
         integral = -500;
      }
   }
   else
   {
      error = 0;
      integral = 0;
      derivative = 0;
   }
   lastError = error;

   *pwm = (uint32_t) (Kp * error) + (Ki * integral) + (Kd * derivative);

   if (*pwm > 500)
   {
      *pwm = 500;
   }

   if (*pwm < 0)
   {
      *pwm = 0;
   }

   if (temperature->current >= 495)
   {
      *pwm = 0;
   }
}
