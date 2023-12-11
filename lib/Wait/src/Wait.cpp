#include <Arduino.h>
#include "Wait.h"

bool Wait::run () {
    if  (interval == 0) return true;

    unsigned long currentMillis = millis();

    if (previousMillis == 0) {
        previousMillis = currentMillis;
        return false;
    }

    if(currentMillis - previousMillis <= interval) return false;

    previousMillis = currentMillis;
    return true;
}
