#pragma once
#include <ArduinoJson.h>
#include <vector>

#include "departure.h"


bool fetchDepartures(std::vector<Departure>& result);
Departure makeDepartureStruct(JsonObject departureData);
void printDeparture(const Departure& departure);
