#include <vector>
#include <algorithm>

#include "departure.h"
#include "departures.h"
#include "transportapi.h"


namespace {
    std::vector<Departure> departures;

    unsigned long lastFetch = 0;
    constexpr unsigned long FETCH_INTERVAL = 40000;

    unsigned long lastUpdate = 0;
    constexpr unsigned long UPDATE_INTERVAL = 10000;
}


void fetchDeparturesIfNeeded() {
    static bool firstFetch = true;
    
    if (!firstFetch && millis() - lastFetch < FETCH_INTERVAL) return;

    std::vector<Departure> fetched;

    if (fetchDepartures(fetched)) {
        departures = std::move(fetched);
    }

    firstFetch = false;
    lastFetch = millis();
}

void updateDepartures() {
    static bool firstUpdate = true;

    if (!firstUpdate && millis() - lastUpdate < UPDATE_INTERVAL) return;

    firstUpdate = false;
    lastUpdate = millis();

    Serial.println("\nUpdating cached departures...");

    if (departures.empty()) return;

    // remove past

    // update order


    for (const Departure& departure : departures) {
        printDeparture(departure);
    }
}
