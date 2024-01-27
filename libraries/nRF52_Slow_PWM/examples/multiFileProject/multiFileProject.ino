/****************************************************************************************************************************
  multiFileProject.ino

  For nRF52-based boards using Adafruit_nRF52_Arduino core
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/nRF52_Slow_PWM
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#define NRF52_SLOW_PWM_VERSION_MIN_TARGET      "NRF52_Slow_PWM v1.2.2"
#define NRF52_SLOW_PWM_VERSION_MIN             1002002

#include "multiFileProject.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "nRF52_Slow_PWM.h"

void setup()
{
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  Serial.println("\nStart multiFileProject on ");
  Serial.println(BOARD_NAME);
  Serial.println(NRF52_SLOW_PWM_VERSION);

#if defined(NRF52_SLOW_PWM_VERSION_MIN)

  if (NRF52_SLOW_PWM_VERSION_INT < NRF52_SLOW_PWM_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(NRF52_SLOW_PWM_VERSION_MIN_TARGET);
  }

#endif
}

void loop()
{
  // put your main code here, to run repeatedly:
}
