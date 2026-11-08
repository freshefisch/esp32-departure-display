#pragma once

void initDisplay();
void drawText(uint8_t pos, uint8_t row, String text);
void drawDepartureIcon(uint8_t row);
void updateDisplay();
void clearBuffer();
void clearDisplay();

// void showLoading();
// void showMessage(const char* message);
