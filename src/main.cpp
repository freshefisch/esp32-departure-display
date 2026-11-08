#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#include "config.h"
#include "wifi.h"
#include "display.h"
#include "transportapi.h"
#include "departures.h"


void setup() {
    Serial.begin(115200);

    initDisplay();
    drawText(0,0, "Connecting to wifi...");
    updateDisplay();

    connectToWifi();

    // if (connectToWifi()) {
    //     Serial.println("Wifi connected");
    //     drawText(0,1, "WiFi connected");
    //     drawText(0,2, ("IP: " + getIp()).c_str());
    // }
    // else {
    //     Serial.println("WiFi failed");
    //     drawText(0,1, "WiFi connection failed");
    //     // What should happen if WiFi is unavailable?
    // }

    // configTime(0, 0, "pool.ntp.org");

    // updateDisplay();

    // fetchDeparture();

    // Serial.println("Setup finished.");
    // drawText(0,3, "Setup finished.");
    // updateDisplay();
    // delay(1000);
}

void loop() {
    // Try reconnecting if needed
    updateWifi();

    // UPDATE DISPLAY HERE
    // TODO
    
    // If not connected, don't fetch departures
    if (!isWifiConnected()) {return;}

    // FETCH DEPARTURES HERE
    fetchDeparturesIfNeeded();

    updateDepartures();
}
