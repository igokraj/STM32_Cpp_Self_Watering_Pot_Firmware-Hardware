#include "main.hpp"
#include "iwdg.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h" // It is not necessary in C++
#include "adc.h"
#include "buttons.hpp"
#include "display_ui.hpp"
#include "system_status.hpp"
#include "stm32f4xx_hal_gpio.h"

// Raw value provided by the capacitive sensor
volatile uint32_t RawHumValue = 0;

// This variable is used for the manual start (it is a flag in the EXTI button handling)
volatile bool ManualButtonStart = 0;

// This variable indicates water level in the container (1 -> container is empty, 0 - > container still have water)
bool ContainerEmpty;


// 0-4095 conversion into 0-100% Humidity value
uint8_t ConvertToPercent(uint32_t rawValue) {
    uint8_t Humidity = (rawValue * 100) / 4095;
    return Humidity;
}

// This is the class for handling output pins (e.g. 3x RGB Led's, MOSFET gate, Buzzer);
class DigitalOutput {
    private:
    GPIO_TypeDef *port_;
    uint16_t pin_;
    
    public:
    DigitalOutput(GPIO_TypeDef *port, uint16_t pin) : port_(port), pin_(pin)
    {
    }

    void on() {
        HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
    }
    void off() {
        HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
    }
};

DigitalOutput RedLed(Red_LED_GPIO_Port, Red_LED_Pin);
DigitalOutput GreenLed(Green_LED_GPIO_Port, Green_LED_Pin);
DigitalOutput BlueLed(Blue_LED_GPIO_Port, Blue_LED_Pin);
DigitalOutput Buzzer(Buzzer_Status_GPIO_Port, Buzzer_Status_Pin);
DigitalOutput MOSFET(Pump_on_GPIO_Port, Pump_on_Pin);

void ApplyOutPuts(SystemStatus_t status) {
    switch (status) {
        case SystemStatus_t::Waiting:
            RedLed.off();
            BlueLed.off();
            GreenLed.off();
            MOSFET.off();
            Buzzer.off();
            break;
        case SystemStatus_t::Watering:
            RedLed.off();
            BlueLed.off();
            GreenLed.on();
            MOSFET.on();
            Buzzer.off();
            break;
        case SystemStatus_t::EmptyContainer:
            RedLed.off();
            BlueLed.on();
            GreenLed.off();
            MOSFET.off();
            Buzzer.off();
            break;
        case SystemStatus_t::Error:
            RedLed.on();
            BlueLed.off();
            GreenLed.off();
            MOSFET.off();
            Buzzer.on();
            // After ~2s watchdog will reset the microcontroller
            break;
    }
}

class Pot {

    private:
    uint8_t DesiredHumidity;
    SystemStatus_t SystemStatus = SystemStatus_t::Waiting;
    
    public: 
    Pot(uint8_t DesiredHumidity) {
        this->DesiredHumidity = DesiredHumidity;
    }

    // ***** GETTERS *****
    // a) Getter for the desired humidity 
    uint8_t GetDesiredHumidity() const { // Getter does not change the parameter (const-correctness)
        return DesiredHumidity;
    }
    // b) Getter for the current system status
    SystemStatus_t GetSystemStatus() const { // Getter does not change the parameter (const-correctness)
        return SystemStatus;
    }
    // *******************


    // System status update logic 
    void UpdateSystem(uint8_t currentHumidity, bool ContainerEmpty) {

        if (!ContainerEmpty) {
        if (currentHumidity > 100) {
                SystemStatus = SystemStatus_t::Error;
            }
        else if (currentHumidity <= DesiredHumidity || ManualButtonStart) {
            SystemStatus = SystemStatus_t::Watering;
        }
        else {
            SystemStatus = SystemStatus_t::Waiting;
        }
        }
        else {
            SystemStatus = SystemStatus_t::EmptyContainer;
        }
    }

    // ***** SETTER *****
    void SetDesiredHumidity(uint8_t DesiredHumidity) {
        this->DesiredHumidity = DesiredHumidity;
    }
    // ******************
};

// Desired humidity set right after start of the system
Pot AloePot(50);


void app_main() {

HAL_IWDG_Refresh(&hiwdg);  // fresh watchdog window - display init takes a moment
DisplayInit();

while (1) {

ContainerEmpty = HAL_GPIO_ReadPin(Water_level_GPIO_Port, Water_level_Pin);

if (btnPlus.Update()) {
    uint8_t newHumidity = AloePot.GetDesiredHumidity() + 10;
    if (newHumidity > 100) {
        newHumidity = 100;
    }
    AloePot.SetDesiredHumidity(newHumidity);
}

if (btnMinus.Update()) {
uint8_t currentHumidity = AloePot.GetDesiredHumidity();
uint8_t newHumidity = (currentHumidity >= 10) ? currentHumidity - 10 : 0;
    AloePot.SetDesiredHumidity(newHumidity);
}

uint8_t humidity = ConvertToPercent(RawHumValue);

AloePot.UpdateSystem(humidity, ContainerEmpty);

ApplyOutPuts(AloePot.GetSystemStatus());

RefreshDisplay(humidity, AloePot.GetDesiredHumidity(), AloePot.GetSystemStatus());

if (AloePot.GetSystemStatus() != SystemStatus_t::Error) {
    HAL_IWDG_Refresh(&hiwdg);
}
}


}


// Callback function for ADC start in ISR
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
if (htim->Instance == TIM6) {
HAL_ADC_Start_IT(&hadc1);
}

}

// Callback for the humidity measurement in ISR
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) {
        RawHumValue = HAL_ADC_GetValue(hadc);
    }
}

// Callback for the Start button EXTI
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

  if (GPIO_Pin == Start_Button_Pin) {
  ManualButtonStart = !ManualButtonStart;
  }
}
