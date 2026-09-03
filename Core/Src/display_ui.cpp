#include "display_ui.hpp"
#include "ssd1306.hpp"
#include <stdio.h>

static Display Oled(&hi2c1);  // change to Display(&hi2c1, 0x3D) if the module uses that address

// Last values put on the screen, used to skip redundant redraws
static uint8_t shownHumidity = 255;
static uint8_t shownDesired = 255;
static SystemStatus_t shownStatus = SystemStatus_t::Error;
static bool everDrawn = false;

// Short state names that fit on one OLED line
static const char *StatusName(SystemStatus_t status) {
    switch (status) {
        case SystemStatus_t::Waiting:        return "WAITING";
        case SystemStatus_t::Watering:       return "WATERING";
        case SystemStatus_t::EmptyContainer: return "NO WATER";
        case SystemStatus_t::Error:          return "ERROR";
    }
    return "";
}

static void Draw(uint8_t humidity, uint8_t desired, SystemStatus_t status) {
    char line[20];

    Oled.clear();

    Oled.text(13, 0, "SELF WATERING POT");
    Oled.hline(0, 10, Display::Width);

    Oled.bitmap(2, 14, IconPlant, 32, 32);

    snprintf(line, sizeof(line), "HUM: %u%%", static_cast<unsigned>(humidity));
    Oled.text(44, 18, line);

    snprintf(line, sizeof(line), "SET: %u%%", static_cast<unsigned>(desired));
    Oled.text(44, 32, line);

    snprintf(line, sizeof(line), "STATE: %s", StatusName(status));
    Oled.text(2, 52, line);

    Oled.flush();
}

void DisplayInit() {
    Oled.init();
}

void RefreshDisplay(uint8_t humidity, uint8_t desired, SystemStatus_t status) {
    if (everDrawn && humidity == shownHumidity && desired == shownDesired && status == shownStatus) {
        return;
    }

    Draw(humidity, desired, status);

    shownHumidity = humidity;
    shownDesired = desired;
    shownStatus = status;
    everDrawn = true;
}
