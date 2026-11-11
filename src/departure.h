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
        // Departure time is essentially rounded to nearest whole number
        long seconds = secondsRemaining() + 30;
        return seconds >= 0 ? seconds / 60 : (seconds - 59) / 60;
    }

    String minutesRemainingStr() const {
        return String(minutesRemaining());
    }

    bool isExpired() const {
        // Departure icon shown for 30 + 10 = 40 seconds
        return secondsRemaining() < -10;
    }
};
