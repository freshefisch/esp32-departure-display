#include <Arduino.h>
#include <U8g2lib.h>

#include "config.h"
#include "wifi.h"
#include "display.h"
#include "transportapi.h"
#include "departures.h"


void setup() {
    Serial.begin(115200);

    initDisplay();
    connectToWifi();

    Serial.println("Setup finished.");
}

void loop() {
    // Try reconnecting if needed
    bool wifiReady = updateWifi();

    bool updatedDepartures = updateDepartures();

    if (updatedDepartures) {
        drawDepartures(getDepartures());
    }

    // If not connected, don't fetch departures
    if (!wifiReady || !isClockSynced()) {return;}

    fetchDeparturesIfNeeded();
}
