#include <Arduino.h>
#include <WiFi.h>

#include "wifi.h"
#include "secrets.h"
#include "clock.h"


namespace {
    bool connecting = false;
    unsigned long connectionStart = 0;
    unsigned long lastAttempt = 0;

    constexpr unsigned long WIFI_TIMEOUT = 15000;
    constexpr unsigned long RETRY_INTERVAL = 30000;
}


void connectToWifi() {
    if (connecting || isWifiConnected()) {
        return;
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    connecting = true;
    connectionStart = millis();
    lastAttempt = connectionStart;

    Serial.println("Connecting to WiFi...");
}

void updateWifi() {
    if (isWifiConnected()) {
        // If we are connected and still "connecting"
        if (connecting) {
            Serial.print("WiFi connected, ");
            Serial.println("IP: " + getIp());
            initClock();
            connecting = false;
        }
        // If connected and done "connecting", do nothing
        return;
    }

    // If we are NOT connected and still "connecting"
    if (connecting) {
        // If we have been trying for longer than WIFI_TIMEOUT, give up
        if (millis() - connectionStart >= WIFI_TIMEOUT) {
            Serial.println("WiFi connection timed out! Retrying in a couple seconds");

            WiFi.disconnect();
            connecting = false;
        }
        return;
    }

    // If we are not connecting and haven't been trying for at least RETRY_INTERVAL
    if (millis() - lastAttempt >= RETRY_INTERVAL) {
        connectToWifi();
    }
}


bool isWifiConnected() {
    return WiFi.status() == WL_CONNECTED;
}


String getIp() {
    return WiFi.localIP().toString();
}


void printWifiNetworks() {
    WiFi.mode(WIFI_STA);

    Serial.println("Scanning WiFi networks...");

    int networks = WiFi.scanNetworks();

    for (int i = 0; i < networks; i++) {
        Serial.printf(
            "SSID: %s | RSSI: %ld dBm | Channel: %ld\n",
            WiFi.SSID(i).c_str(),
            WiFi.RSSI(i),
            WiFi.channel(i)
        );
    }

    WiFi.scanDelete();
}
