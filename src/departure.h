#pragma once
#include <Arduino.h>
#include "clock.h"

struct Departure {
    String line;
    String destination;
    time_t departureTime;
    int delay;
    
    time_t expectedDepartureTime() const {
        return departureTime + delay * 60;
    }

    long secondsRemaining() const {
        return static_cast<long>(expectedDepartureTime() - getCurrentTime());
    }

    int minutesRemaining() const {
        return secondsRemaining() / 60;
    }

    String minutesRemainingStr() const {
        return String(minutesRemaining());
    }
};
