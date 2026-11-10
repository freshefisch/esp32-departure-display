#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include "transportapi.h"
#include "certificates.h"
#include "departure.h"


namespace {

    constexpr char API_URL[] = "https://transport.opendata.ch/v1/stationboard";
    constexpr char FETCH_NUM[] = "10";
    constexpr char STATION_ID[] = "8503059";
    constexpr const char* FIELDS[] = {
        "stationboard/category",
        "stationboard/number",
        "stationboard/to",
        "stationboard/stop/departureTimestamp",
        "stationboard/stop/delay",
        "stationboard/stop/platform",
        "stationboard/stop/prognosis/departure"
    };
}


bool fetchDepartures(std::vector<Departure>& deps) {
    Serial.print("\nFetching new departures... ");

    String url = String(API_URL) + "?id=" + STATION_ID + "&limit=" + FETCH_NUM;

    for (const char* field : FIELDS) {
        url += "&fields[]=";
        url += field;
    }

    WiFiClientSecure client;
    client.setCACert(ROOT_CA);
    HTTPClient http;

    if (!http.begin(client, url)) {
        Serial.println("Failed to initialize HTTPS");
        return false;
    }

    http.setTimeout(10000);

    int status = http.GET();

    if (status != HTTP_CODE_OK) {
        Serial.println("Request failed with code " + http.errorToString(status));
        http.end();
        return false;
    }
    
    String response = http.getString();
    http.end();

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
        Serial.printf("JSON parsing failed: %s\n ", error.c_str());
        return false;
    }

    Serial.printf("Got %d new departures\n", doc["stationboard"].size());
    JsonArray stationboard = doc["stationboard"];

    for (JsonObject departureData : stationboard) {
        Departure departure = makeDepartureStruct(departureData);
        deps.push_back(departure);
    }

    return true;
}


Departure makeDepartureStruct(JsonObject departureData) {
    Departure departure;
    
    String line = "";
    String category = departureData["category"].as<String>();
    if (category != "T" && category != "B") {
        line += category;
    }
    line += departureData["number"].as<String>();
    
    departure.line = line;
    departure.destination = departureData["to"].as<String>();
    departure.departureTime = departureData["stop"]["departureTimestamp"];
    departure.delay = departureData["stop"]["delay"];

    return departure;
}
