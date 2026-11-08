#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#include "display.h"
#include "config.h"


namespace {
    U8G2_SSD1309_128X64_NONAME0_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

    constexpr uint8_t LINE_NUM = 6;
    constexpr uint8_t POS_NUM = 21;
    constexpr uint8_t LINE_HEIGHT = 9;
    constexpr uint8_t LINE_SPACING = 2;
    constexpr uint8_t TOP_SPACING = 1;

    constexpr uint8_t TRAM_ICON[] = {
        0b00011110,
        0b00111111,
        0b00100001,
        0b00100001,
        0b00111111,
        0b00101101,
        0b00111111,
        0b00010010
    };
}


void initDisplay() {
    delay(1500);

    Serial.println("Setting up display");

    pinMode(LED_PIN, OUTPUT);

    Wire.begin(SDA_PIN, SCL_PIN);
    display.begin();

    display.setFont(u8g2_font_profont11_tr);
    display.setFontPosTop();
    display.clearBuffer();
    display.sendBuffer();

    Serial.println("Display set up");    
}


void drawText(uint8_t pos, uint8_t row, String text) {
    uint8_t y = TOP_SPACING + row * (LINE_HEIGHT + LINE_SPACING);
    display.drawStr(pos, y, text.c_str());
}


void drawDepartureIcon(uint8_t row) {
    uint8_t y = TOP_SPACING + row * (LINE_HEIGHT + LINE_SPACING);
    display.drawXBM((128-6), y, 6, 8, TRAM_ICON);
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
