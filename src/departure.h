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
        long seconds = secondsRemaining();
        // Above 20s, round up. Below, set to 0
        return seconds <= 20 ? 0 : (seconds + 59) / 60;
    }

    String minutesRemainingStr() const {
        return String(minutesRemaining());
    }

    bool isExpired() const {
        // Departure icon shown for 20 + 10 = 30 seconds
        return secondsRemaining() < -10;
    }
};

struct FormattedDeparture {
    String line;
    String destination;
    String etd;
    bool isDeparting;
};
