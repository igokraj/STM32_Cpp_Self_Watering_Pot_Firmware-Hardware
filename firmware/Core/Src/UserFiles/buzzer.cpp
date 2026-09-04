#include "buzzer.hpp"

#define INTERVAL_BETWEEN_BEEPS  60000 // 1 min
#define TIMES_OF_BEEP 3

static uint32_t lastPeriodTick = 0;
static uint32_t lastBeepTick = 0;
static uint8_t beepCount = 0;
static uint8_t isBeeping = 0;
static uint8_t beepState = 0;

void BuzzerBeep() {

    uint32_t currentTick = HAL_GetTick();

    // Start a new burst once the interval between bursts has elapsed
    if (!isBeeping && (currentTick - lastPeriodTick >= INTERVAL_BETWEEN_BEEPS)) {
        lastPeriodTick = currentTick;
        isBeeping = 1;
        beepCount = 0;
        beepState = 1;
        lastBeepTick = currentTick;
        Buzzer.on();
    }

    // Fast beeping (100 ms ON, 100 ms OFF)
    if (isBeeping) {
        if (currentTick - lastBeepTick >= 100) {
            lastBeepTick = currentTick;


            if (beepState == 1) {
                Buzzer.off();
                beepState = 0;
                beepCount++;

                // Finish if buzzer beeped 3 times
                if (beepCount >= TIMES_OF_BEEP) {
                    isBeeping = 0;
                }
            } else {
                Buzzer.on();
                beepState = 1;
            }
        }
    }
}
