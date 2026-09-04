#pragma once
#include "system_status.hpp"
#include <stdint.h>

// OLED user interface. The display object and the whole screen layout live in display_ui.cpp, so the application only deals with these two calls.

void DisplayInit();

// Redraws the screen, but only when one of the shown values actually changed (a full frame costs a full I2C transfer, so it is not worth doing every loop).
void RefreshDisplay(uint8_t humidity, uint8_t desired, SystemStatus_t status);
