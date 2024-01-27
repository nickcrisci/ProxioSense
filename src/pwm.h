#pragma once

#include "NRF52_MBED_TimerInterrupt.hpp"

void TimerHandler ();

void setVibrationInterval ();

#define HW_TIMER_INTERVAL_US 100L