#include <Arduino.h>
#include <time.h>
#include "clock.h"

void initClock() {
    configTime(0, 0, "pool.ntp.org");
}

bool isClockSynced() {
    return time(nullptr) > 1700000000;
}

time_t getCurrentTime() {
    return time(nullptr);
}
