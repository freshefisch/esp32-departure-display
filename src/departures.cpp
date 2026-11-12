#include <vector>
#include <algorithm>

#include "departure.h"
#include "departures.h"
#include "transportapi.h"


namespace {
    std::vector<Departure> departures;

    unsigned long lastFetch = 0;
    constexpr unsigned long FETCH_INTERVAL = 120000;
    constexpr unsigned long MIN_FETCH_INTERVAL = 30000;
    constexpr size_t MIN_DEPARTURES = 5;

    unsigned long lastUpdate = 0;
    constexpr unsigned long UPDATE_INTERVAL = 10000;
}


FetchResult fetchDeparturesIfNeeded() {
    static bool firstFetch = true;
    
    // If less than MIN_DEPARTURES are chached, then fetch after MIN_FETCH_INTERVAL. Otherwise FETCH_INTERVAL
    unsigned long interval = departures.size() <= MIN_DEPARTURES
        ? MIN_FETCH_INTERVAL : FETCH_INTERVAL;

    if (!firstFetch && millis() - lastFetch < interval) {
        return FetchResult::Skipped;
    }

    std::vector<Departure> fetched;

    bool fetchSucessful = fetchDepartures(fetched);

    if (fetchSucessful) {
        departures = std::move(fetched);
    }

    firstFetch = false;
    lastFetch = millis();

    return fetchSucessful
        ? FetchResult::Success
        : FetchResult::Failed;
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
