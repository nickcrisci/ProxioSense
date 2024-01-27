/****************************************************************************************************************************
  nRF52_Slow_PWM_ISR.h
  For nRF52-based boards using Adafruit_nRF52_Arduino core
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/nRF52_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one  RP2040-based timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.2.2

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      25/09/2021 Initial coding for nRF52-based boards using Adafruit_nRF52_Arduino core
  1.1.0   K Hoang      10/11/2021 Add functions to modify PWM settings on-the-fly
  1.2.0   K.Hoang      07/02/2022 Fix `multiple-definitions` linker error. Improve accuracy. Optimize code. Fix bug
  1.2.1   K Hoang      03/03/2022 Fix `DutyCycle` and `New Period` display bugs. Display warning only when debug level > 3
  1.2.2   K.Hoang      26/10/2022 Add support to Seeed_XIAO_NRF52840 and Seeed_XIAO_NRF52840_SENSE
*****************************************************************************************************************************/

#pragma once

#ifndef NRF52_SLOW_PWM_ISR_H
#define NRF52_SLOW_PWM_ISR_H


#include "nRF52_Slow_PWM_ISR.hpp"
#include "nRF52_Slow_PWM_ISR_Impl.h"

#endif    // NRF52_SLOW_PWM_ISR_H


