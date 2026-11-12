#pragma once
#include <ArduinoJson.h>

#include "departure.h"
#include "transportapi.h"

FetchResult fetchDeparturesIfNeeded();
bool updateDepartures();
const std::vector<Departure>& getDepartures();
