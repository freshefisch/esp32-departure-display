#include <Arduino.h>
#include <U8g2lib.h>

#include "main.h"
#include "config.h"
#include "wifi.h"
#include "display.h"
#include "transportapi.h"
#include "departures.h"


namespace {
    bool apiAvailable = true;
}

void setup() {
    Serial.begin(115200);

    initDisplay();
    connectToWifi();

    Serial.println("Setup finished.");
}


void loop() {
    // Try reconnecting if needed
    bool wifiReady = updateWifi();

    bool departuresChanged = updateDepartures();

    DisplayState currentState = getDisplayState();
    static DisplayState previousState = DisplayState::Connecting;

    bool stateChanged = currentState != previousState;

    if (stateChanged || departuresChanged) {
        drawDisplay(currentState, getDepartures());
        previousState = currentState;
    }

    // If not connected, don't fetch departures
    if (!wifiReady || !isClockSynced()) {return;}

    FetchResult fetchResult = fetchDeparturesIfNeeded();

    if (fetchResult != FetchResult::Skipped) {
        apiAvailable = (fetchResult == FetchResult::Success);
    }
}


DisplayState getDisplayState() {
    if (!getDepartures().empty()) {
        return DisplayState::Departures;
    }

    if (!isWifiConnected()) {
        return DisplayState::NoWifi;
    }

    if (!isClockSynced()) {
        return DisplayState::Connecting;
    }

    if (!apiAvailable) {
        return DisplayState::ApiUnavailable;
    }

    return DisplayState::Loading;
}
