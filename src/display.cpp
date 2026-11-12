#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <vector>

#include "departure.h"
#include "display.h"
#include "config.h"
#include "wifi.h"


namespace {
    constexpr int OLED_SCLK = 4;
    constexpr int OLED_MOSI = 5;
    constexpr int OLED_DC   = 6;
    constexpr int OLED_RST  = 7;
    constexpr int OLED_CS   = 10;

    U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI display(
        U8G2_R0,
        OLED_CS,
        OLED_DC,
        OLED_RST
    );

    constexpr uint8_t LINE_NUM = 5;
    constexpr uint8_t LINE_HEIGHT = 12;
    constexpr uint8_t LINE_SPACING = 1;
    constexpr uint8_t TOP_SPACING = 0;
    constexpr uint16_t DISPLAY_WIDTH = 256;
    constexpr uint16_t DISPLAY_HEIGHT = 64;

    constexpr uint8_t COLUMN_MINWIDTH_LINE = 32;
    constexpr uint8_t COLUMN_MINWIDTH_ETD = 8;
    constexpr uint8_t COLUMN_GAP = 8;

    static const uint8_t DEPARTING_ICON_8x12[] = {
        0b01111110,
        0b10000001,
        0b11111111,
        0b10000001,
        0b10000001,
        0b10000001,
        0b10000001,
        0b11111111,
        0b10111101,
        0b11111111,
        0b01000010,
        0b00000000 
    };
}

extern "C" {
    extern const uint8_t vbz_font[];
}


void initDisplay() {
    delay(1500);

    Serial.println("Setting up display");

    SPI.begin(OLED_SCLK, -1, OLED_MOSI, OLED_CS);
    display.begin();
    display.setContrast(90);
    display.setFlipMode(1);
    display.clearBuffer();

    display.setFont(vbz_font);
    display.setFontPosTop();

    Serial.println("Display set up");
}


void drawText(uint8_t pos, uint8_t row, String text, bool center) {
    if (center) {
        pos = (DISPLAY_WIDTH - textWidth(text) ) / 2;
    }
    uint8_t y = TOP_SPACING + row * (LINE_HEIGHT + LINE_SPACING);
    display.drawUTF8(pos, y, text.c_str());
}


void updateDisplay() {
    display.sendBuffer();
}


void clearBuffer() {
    display.clearBuffer();
}


void clearDisplay() {
    display.clearBuffer();
    display.sendBuffer();
}


void drawDepartures(const std::vector<Departure>& departures) {
    Serial.println("--- Drawing departures...");

    uint16_t columnWidthLine = COLUMN_MINWIDTH_LINE;
    uint16_t columnWidthEtd = COLUMN_MINWIDTH_ETD;

    uint8_t i = 0;
    std::vector<FormattedDeparture> formattedDepartures;
    formattedDepartures.reserve(min(departures.size(), static_cast<size_t>(LINE_NUM)));


    // CREATE FormattedDeparture STRUCTS WITH FORMATTED DATA

    for (const Departure& departure : departures) {
        FormattedDeparture formDeparture;

        formDeparture.line = departure.line;

        columnWidthLine = max(columnWidthLine, textWidth(formDeparture.line));

        formDeparture.destination = departure.destination;

        String etd = "";
        int minutesRemaining = departure.minutesRemaining();

        if (minutesRemaining <= 0) {
            formDeparture.isDeparting = true;
            etd += "0";
        } else {
            formDeparture.isDeparting = false;

            if (minutesRemaining > 99) {
                etd += ">99";
            } else {
                etd += departure.minutesRemainingStr();
            }
        }
        etd += "'";
        formDeparture.etd = etd;

        columnWidthEtd = max(columnWidthEtd, textWidth(etd));

        formattedDepartures.push_back(formDeparture);

        i++;
        if (i >= LINE_NUM) break;
    }


    // DRAW DEPARTURES

    uint8_t currentRow = 0;
    for (const FormattedDeparture& dep : formattedDepartures) {
        uint8_t y = TOP_SPACING + currentRow * (LINE_HEIGHT + LINE_SPACING);

        int16_t lineX = columnWidthLine - textWidth(dep.line);
        display.drawUTF8(lineX, y, dep.line.c_str());

        int16_t destX = columnWidthLine + COLUMN_GAP;
        int16_t destRightX = DISPLAY_WIDTH - columnWidthEtd - COLUMN_GAP;

        if (destRightX > destX) {
            display.setClipWindow(destX, 0, destRightX, DISPLAY_HEIGHT);
            display.drawUTF8(destX, y, dep.destination.c_str());
            display.setMaxClipWindow();
        }

        if (dep.isDeparting) {
            display.drawXBM(
                DISPLAY_WIDTH - 8,
                y,
                8,
                12,
                DEPARTING_ICON_8x12
            );
        }
        else {
            int16_t etdX = DISPLAY_WIDTH - textWidth(dep.etd);
            display.drawUTF8(etdX, y, dep.etd.c_str());
        }

        currentRow += 1;
        if (currentRow >= LINE_NUM) break;
    }

    // Print all departures for debugging
    for (const Departure& departure : departures) {
        printDeparture(departure);
    }
}


void printDeparture(const Departure& departure) {
    Serial.printf("%s %s %d (%ld s)\n", departure.line.c_str(), departure.destination.c_str(), departure.minutesRemaining(), departure.secondsRemaining());
}


void drawDisplay(DisplayState state, const std::vector<Departure>& departures) {
    Serial.println("-- Drawing display...");
    display.clearBuffer();

    switch(state) {
        case DisplayState::Departures:
            drawDepartures(departures);
            break;
        
        case DisplayState::NoWifi:
            drawText(0, 1, "Connecting to WiFi...", true);
            drawText(0, 3, "SSID: " + getWifiSSID(), true);
            break;
        
        case DisplayState::Connecting:
            drawText(0, 2, "Connecting...", true);
            break;
        
        case DisplayState::Loading:
            drawText(0, 2, "Loading departures...", true);
            break;
        
        case DisplayState::ApiUnavailable:
            drawText(0, 2, "Departures unavailable", true);
            break;
    }

    display.sendBuffer();
}

uint16_t textWidth (const String& text) {
    return static_cast<uint16_t>(display.getUTF8Width(text.c_str()));
}