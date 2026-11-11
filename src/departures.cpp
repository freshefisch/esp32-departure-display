#include <vector>
#include <algorithm>

#include "departure.h"
#include "departures.h"
#include "transportapi.h"


namespace {
    std::vector<Departure> departures;

    unsigned long lastFetch = 0;
    constexpr unsigned long FETCH_INTERVAL = 120000;

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


bool updateDepartures() {
    static bool firstUpdate = true;

    if (departures.empty() || (!firstUpdate && millis() - lastUpdate < UPDATE_INTERVAL)) return false;

    firstUpdate = false;
    lastUpdate = millis();

    Serial.println("\n- Updating cached departures...");

    if (departures.empty()) return false;

    departures.erase(std::remove_if(
        departures.begin(),
        departures.end(),
        [](const Departure& d) {
            return d.isExpired();
        }
    ), departures.end());
    
    std::sort(
        departures.begin(),
        departures.end(),
        [](const Departure& a, const Departure& b) {
            if (a.secondsRemaining() != b.secondsRemaining()) {
                return a.secondsRemaining() < b.secondsRemaining();
            }
            // if departure times match, sort by destination
            // ensure consistent sorting
            return a.destination < b.destination;
        }
    );

    return true;
}

const std::vector<Departure>& getDepartures() {
    return departures;
}
