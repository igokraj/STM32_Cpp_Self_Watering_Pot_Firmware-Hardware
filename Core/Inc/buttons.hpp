#pragma once
#include "gpio.h"

class Button {
  private:
    GPIO_TypeDef *port;
    uint16_t      pin;
    GPIO_PinState stableState;
    uint8_t       counter;
    uint32_t      pressStartTick;
    uint32_t      lastRepeatTick;

  public:
    Button(GPIO_TypeDef *port,
           uint16_t pin,
           GPIO_PinState stableState,
           uint8_t counter,
           uint32_t pressStartTick,
           uint32_t lastRepeatTick) {
        this->port = port;
        this->pin = pin;
        this->stableState = stableState;
        this->counter = counter;
        this->pressStartTick = pressStartTick; 
        this->lastRepeatTick = lastRepeatTick;
    }

    // Returns 1 on a debounced press event, or on an auto-repeat tick while held, else 0
    uint8_t Update();
};


extern Button btnPlus;
extern Button btnMinus;
