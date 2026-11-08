#pragma once
#include <ArduinoJson.h>

#include "departure.h"
#include "departures.h"

void fetchDeparturesIfNeeded();
void updateDepartures();