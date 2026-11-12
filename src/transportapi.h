#pragma once
#include <ArduinoJson.h>
#include <vector>

#include "departure.h"

enum class FetchResult {
    Skipped,
    Success,
    Failed
};

bool fetchDepartures(std::vector<Departure>& result);
Departure makeDepartureStruct(JsonObject departureData);
