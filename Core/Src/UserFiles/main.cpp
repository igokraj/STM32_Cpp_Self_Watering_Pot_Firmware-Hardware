#include "main.hpp"
#include "iwdg.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "buttons.hpp"
#include "display_ui.hpp"
#include "system_status.hpp"
#include "stm32f4xx_hal_gpio.h"

// Raw value provided by the capacitive sensor
volatile uint32_t RawHumValue = 0;

// This variable is used for the manual start (it is a flag in the EXTI button handling)
volatile bool ManualButtonStart = 0;

// This variable indicates water level in the container (1 -> container is empty, 0 -> container still has water)
bool ContainerEmpty;

// A healthy, properly connected sensor never sits at either end of the ADC range, so a reading outside these bounds means the sensor failed (broken wire? no power?)
#define SENSOR_RAW_VALUE_MIN 50
#define SENSOR_RAW_VALUE_MAX 4045
bool SensorFailed(uint32_t RawValue) {
    if (RawValue < SENSOR_RAW_VALUE_MIN || RawValue > SENSOR_RAW_VALUE_MAX) {
     return true;
    }
    else {
        return false; 
    }
}



// 0-4095 conversion into 0-100% Humidity value
uint8_t ConvertToPercent(uint32_t rawValue) {
    uint8_t Humidity = (rawValue * 100) / 4095;
    return Humidity;
}

// This is the class for handling output pins (e.g. 3x RGB LEDs, MOSFET gate, Buzzer)
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
            break;
    }
}

class Pot {

    private:

    uint8_t DesiredHumidity;
    SystemStatus_t SystemStatus = SystemStatus_t::Waiting;

    // static const in the class => one value shared by the whole class, not a per-object copy
    static const uint8_t WateringOffset = 15; // if Humidity <= DesiredHumidity - WateringOffset -> Watering
    static const uint32_t MaxWateringMs = 15000; // maximum continuous watering time, if this limit is exceeded, the system will return an error.
    uint32_t WateringStartTick = 0;

    bool SystemFailed = false;
    
    public: 

    Pot(uint8_t DesiredHumidity) {
        this->DesiredHumidity = DesiredHumidity;
    }

    // ***** GETTERS *****
    // a) Getter for the desired humidity 
    uint8_t GetDesiredHumidity() const { // const method - does not modify the object (const-correctness)
        return DesiredHumidity;
    }
    // b) Getter for the current system status
    SystemStatus_t GetSystemStatus() const { // const method - does not modify the object (const-correctness)
        return SystemStatus;
    }
    // *******************


    /* Decides the next SystemStatus, checked in priority order:
    a latched pump fault (only the Start button can clear it) beats a bad sensor reading, which beats an empty container, which beats the humidity threshold itself. Watering is capped at MaxWateringMs so a stuck sensor cannot leave the pump running forever */
    void UpdateSystem(uint8_t currentHumidity, bool ContainerEmpty, bool SensorFailed, bool ManualButtonStart) {

        if (ManualButtonStart) { 
            Clear_SystemFailed(); 
        }
        if (SystemFailed) {
            SystemStatus = SystemStatus_t::Error;
            return;
        }

        if (SensorFailed) {
                SystemStatus = SystemStatus_t::Error;
                WateringStartTick = 0;
                return;
            }
        if (ContainerEmpty) {
                SystemStatus = SystemStatus_t::EmptyContainer;
                WateringStartTick = 0;
                return;
        }
        bool PlantNeedsWater = (currentHumidity <= DesiredHumidity - WateringOffset || ManualButtonStart);

        if (PlantNeedsWater) {
            if (SystemStatus == SystemStatus_t::Watering) {
                if (HAL_GetTick() - WateringStartTick > MaxWateringMs) {
                    SystemStatus = SystemStatus_t::Error;
                    SystemFailed = true; // // pump ran too long - STOP the system
                }
            }
            else {
                WateringStartTick = HAL_GetTick();
                SystemStatus = SystemStatus_t::Watering;
            }
        }
        else {
            SystemStatus = SystemStatus_t::Waiting;
            WateringStartTick = 0;
        }
    }

    // ***** SETTER *****
    void SetDesiredHumidity(uint8_t DesiredHumidity) {
        this->DesiredHumidity = DesiredHumidity;
    }
    // ******************

     void Clear_SystemFailed() {
        SystemFailed = false;
    }
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

// Why did i use raw variable here? 
// RawHumValue is volatile and can change under an ADC interrupt at any point, so it is snapshotted once here - otherwise ConvertToPercent and SensorFailed could end up judging two different readings within the same decision
uint32_t raw = RawHumValue;
uint8_t humidity = ConvertToPercent(raw);
AloePot.UpdateSystem(humidity, ContainerEmpty, SensorFailed(raw), ManualButtonStart);

ApplyOutPuts(AloePot.GetSystemStatus());

RefreshDisplay(humidity, AloePot.GetDesiredHumidity(), AloePot.GetSystemStatus());

HAL_IWDG_Refresh(&hiwdg); // watchdog only guards against a hung main loop

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
