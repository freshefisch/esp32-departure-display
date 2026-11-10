#pragma once
#include <ArduinoJson.h>

#include "departure.h"

void fetchDeparturesIfNeeded();
bool updateDepartures();
const std::vector<Departure>& getDepartures();
