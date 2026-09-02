#include "buttons.hpp"

#define DEBOUNCE_TICKS  2   // number of consecutive stable samples required before a state change counts

// These values are used in mechanism for rapidly increasing the value while holding down the button
#define HOLD_DELAY_MS 500 // -> How long user needs to hold the button pressed for the mechanism to initialize (button pressed ... 500 ms ... mechanism initialization)
#define REPEAT_INTERVAL_MS 100 // -> Intervals between value changes while the mechanism is running (100 RPM ... 150 ms ... 200 RPM)

uint8_t Button::Update()
{
  GPIO_PinState raw = HAL_GPIO_ReadPin(port, pin);
  uint32_t now = HAL_GetTick();

  // The reading is changing - it might be a real press/release, or just bouncing
  if (raw != stableState)
  {
    counter = counter + 1;

    // Wait until the new reading has been stable for DEBOUNCE_TICKS in a row
    if (counter >= DEBOUNCE_TICKS)
    {
      stableState = raw;
      counter = 0;

      if (raw == GPIO_PIN_RESET)
      {
        // Fresh press - remember when it started, report it right away
        pressStartTick = now;
        lastRepeatTick = now;
        return 1;
      }
      else
      {
        // Released
        pressStartTick = 0;
      }
    }
    return 0;
  }

  // Reading is stable - nothing changing, reset the debounce counter
  counter = 0;

  // Still held down - check whether it's time for an auto-repeat event
  if (stableState == GPIO_PIN_RESET && pressStartTick != 0)
  {
    if (now - pressStartTick >= HOLD_DELAY_MS)
    {
      if (now - lastRepeatTick >= REPEAT_INTERVAL_MS)
      {
        lastRepeatTick = now;
        return 1;
      }
    }
  }

  return 0;
}
