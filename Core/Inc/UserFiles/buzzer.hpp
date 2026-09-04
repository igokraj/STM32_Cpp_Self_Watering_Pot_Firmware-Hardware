#pragma once
#include "digital_output.hpp"

extern DigitalOutput Buzzer;

// Beeps 3 short times, then stays silent until the next interval elapses
void BuzzerBeep();
