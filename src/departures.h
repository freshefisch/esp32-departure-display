#pragma once
#include <ArduinoJson.h>

#include "departure.h"

void fetchDeparturesIfNeeded();
void updateDepartures();
const std::vector<Departure>& getDepartures();
