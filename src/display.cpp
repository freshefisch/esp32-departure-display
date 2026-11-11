#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <vector>

#include "departure.h"
#include "display.h"
#include "config.h"


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

    constexpr uint8_t COLUMN_WIDTH_LINE = 32;
    constexpr uint8_t COLUMN_WIDTH_ETD = 32;
    constexpr uint8_t COLUMN_GAP = 8;
    constexpr uint8_t COLUMN_WIDTH_DESTINATION = DISPLAY_WIDTH - COLUMN_WIDTH_ETD - COLUMN_WIDTH_LINE - 2*COLUMN_GAP;

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
    display.setFlipMode(1);
    display.clearBuffer();

    display.setFont(vbz_font);
    display.setFontPosTop();

    drawText(12, 3, "Departure Display Starting...");
    display.sendBuffer();

    Serial.println("Display set up");
}


void drawText(uint8_t pos, uint8_t row, String text) {
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

    display.clearBuffer();

    uint8_t currentRow = 0;
    for (const Departure& departure : departures) {
        uint8_t y = TOP_SPACING + currentRow * (LINE_HEIGHT + LINE_SPACING);
        
        // TODO: Adjust column widths if there is a value larger than default

        // Line
        uint16_t lineX = COLUMN_WIDTH_LINE - display.getUTF8Width(departure.line.c_str());
        display.drawUTF8(lineX, y, departure.line.c_str());

        // Destination
        // TODO: Trunctuate or scroll line if its too wide
        uint16_t destX = COLUMN_WIDTH_LINE + COLUMN_GAP;
        display.drawUTF8(destX, y, departure.destination.c_str());

        // ETD
        if (departure.minutesRemaining() == 0) {
            display.drawXBM(
                DISPLAY_WIDTH - 8,
                y,
                8,
                12,
                DEPARTING_ICON_8x12
            );
        }
        else {
            String etd = departure.minutesRemainingStr() + "'";
            uint16_t etdX = DISPLAY_WIDTH - display.getUTF8Width(etd.c_str());
            display.drawUTF8(etdX, y, etd.c_str());
        }

        currentRow += 1;
        if (currentRow >= LINE_NUM) break;
    }

    // Print all departures for debugging
    for (const Departure& departure : departures) {
        printDeparture(departure);
    }

    display.sendBuffer();
}


void printDeparture(const Departure& departure) {
    Serial.printf("%s %s %d (%ld s)\n", departure.line.c_str(), departure.destination.c_str(), departure.minutesRemaining(), departure.secondsRemaining());
}
