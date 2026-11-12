#pragma once

#include <vector>

#include "departure.h"

enum class DisplayState {
    Departures,
    Connecting,
    NoWifi,
    Loading,
    ApiUnavailable
};

void initDisplay();
void drawText(uint8_t pos, uint8_t row, String text, bool center=false) ;
void updateDisplay();
void clearBuffer();
void clearDisplay();
void drawDepartures(const std::vector<Departure>& departures);
void printDeparture(const Departure& departure);
void drawDisplay(DisplayState state, const std::vector<Departure>& departures);
