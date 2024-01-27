#include "pwm.h"

volatile uint32_t startMillis;

// For PWM_Value from 0-255.You can change to 1024 or 2048
#define MAX_PWM_VALUE         256

#define MAPPING_TABLE_SIZE    (MAX_PWM_VALUE / 10) + 1

// You have to calibrate and update this mapping table
float mappingTable[MAPPING_TABLE_SIZE] =
{
  0.0,     3.281,   6.860,  10.886,  15.285,  20.355,  26.096,  32.732,  40.785,  50.180,
  62.557,  79.557, 104.461, 136.075, 163.066, 181.930, 195.724, 207.132, 216.228, 223.684,
  230.395, 236.136, 241.206, 245.680, 249.781, 253.509
};

#define LED_TOGGLE_INTERVAL_MS        500L

#define NUMBER_ISR_TIMERS         16

typedef void (*irqCallback)  ();

typedef struct
{
  int16_t       PWM_Value;          // Writing negative value to stop and free this PWM
  int16_t       PWM_PremapValue;    // To detect if use the same PWM_Value setting => don't do anything
  uint16_t      pin;
  uint16_t      countPWM;
  bool          beingUsed;
} ISRTimerData;


volatile ISRTimerData curISRTimerData[NUMBER_ISR_TIMERS] =
{
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
  { 0, 0, 0, 0, false },
};

void doingSomething(int index)
{
  // First check if enabled and pin != 0
  if ( (curISRTimerData[index].beingUsed) && (curISRTimerData[index].PWM_Value > 0) && (curISRTimerData[index].pin != 0) )
  {
    // Divide the time into MAX_PWM_VALUE slots.
    // PWM_Value = 0 => no digitalWrite(pin, 1)
    // PWM_Value > 0 => digitalWrite(pin, 1) from countPWM = 0 to countPWM = PWM_Value

    if (curISRTimerData[index].countPWM == 0)
    {
      if (curISRTimerData[index].PWM_Value > 0)
        digitalWrite(curISRTimerData[index].pin, 1);
      else
        digitalWrite(curISRTimerData[index].pin, 0);
    }
    else if (curISRTimerData[index].countPWM == curISRTimerData[index].PWM_Value)
    {
      digitalWrite(curISRTimerData[index].pin, 0);
    }
  }

  curISRTimerData[index].countPWM = (curISRTimerData[index].countPWM + 1) % MAX_PWM_VALUE;
}

void TimerHandler()
{
  static bool toggle  = false;
  static int timeRun  = 0;

  for (int i = 0; i < NUMBER_ISR_TIMERS; i++)
  {
    doingSomething(i);
  }

  // Toggle LED every LED_TOGGLE_INTERVAL_MS = 500ms = 0.5s
  if (++timeRun == ((LED_TOGGLE_INTERVAL_MS * 1000) / HW_TIMER_INTERVAL_US) )
  {
    timeRun = 0;

    //timer interrupt toggles pin LED_BUILTIN
    digitalWrite(LED_BUILTIN, toggle);
    toggle = !toggle;
  }
}


void fakeAnalogWrite(uint16_t pin, uint16_t value)
{
  uint16_t localValue;
  uint16_t localIndex = 0;

  // First check if already got that pin, then just update the PWM_Value
  for (uint16_t i = 0; i < NUMBER_ISR_TIMERS; i++)
  {
    if ( (curISRTimerData[i].beingUsed) && (curISRTimerData[i].pin == pin) )
    {
      localValue = (value < MAX_PWM_VALUE) ? value : MAX_PWM_VALUE;

      if (curISRTimerData[i].PWM_PremapValue == localValue)
      {
        return;
      }
      else if (curISRTimerData[i].PWM_Value >= 0)
      {
        curISRTimerData[i].PWM_PremapValue = localValue;

        // Mapping to corect value
        if ( ( localValue == 0) || ( localValue == MAX_PWM_VALUE - 1) )
        {
          // Keep MAX_PWM_VALUE
          curISRTimerData[i].PWM_Value = localValue;
        }
        else
        {
          // Get the mapping index
          for (uint16_t j = 0; j < MAPPING_TABLE_SIZE; j++)
          {
            if ( (float) localValue < mappingTable[j])
            {
              localIndex = j - 1;
              break;
            }
          }

          // Can use map() function
          // Can use map() function
          curISRTimerData[i].PWM_Value = (uint16_t) ( (localIndex * 10 ) +
                                                      ( (value - mappingTable[localIndex]) * 10 ) /  (mappingTable[localIndex + 1] - mappingTable[localIndex]) );
        }
      }
      else
      {
        curISRTimerData[i].beingUsed      = false;
        curISRTimerData[i].pin            = 0;
        curISRTimerData[i].PWM_Value      = 0;
      }

      // Reset countPWM
      curISRTimerData[i].countPWM = 0;

      return;
    }
  }

  for (uint16_t i = 0; i < NUMBER_ISR_TIMERS; i++)
  {
    if (!curISRTimerData[i].beingUsed)
    {
      curISRTimerData[i].beingUsed    = true;
      curISRTimerData[i].pin          = pin;

      // Mapping to corect value
      localValue = (value < MAX_PWM_VALUE) ? value : MAX_PWM_VALUE;

      if ( ( localValue == 0) || ( localValue == MAX_PWM_VALUE - 1) )
      {
        // Keep MAX_PWM_VALUE
        curISRTimerData[i].PWM_Value = localValue;
      }
      else
      {
        // Get the mapping index
        for (uint16_t j = 0; j < MAPPING_TABLE_SIZE; j++)
        {
          if ( (float) localValue < mappingTable[j])
          {
            localIndex = j - 1;
            break;
          }
        }

        // Can use map() function
        // Can use map() function
        curISRTimerData[i].PWM_Value = (uint16_t) ( (localIndex * 10 ) +
                                                    ( (value - mappingTable[localIndex]) * 10 ) /  (mappingTable[localIndex + 1] - mappingTable[localIndex]) );
      }

      curISRTimerData[i].countPWM     = 0;

      pinMode(pin, OUTPUT);

      return;
    }
  }
}

void setVibrationInterval () {
  while (true) {
    fakeAnalogWrite(7, 150);
    delay(2000);
    fakeAnalogWrite(7, 255);
    delay(2000);
  }
}

