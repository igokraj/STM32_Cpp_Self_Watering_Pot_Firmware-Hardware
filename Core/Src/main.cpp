#include "main.hpp"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h" // It is not necessary in C++
#include "main.h" 
#include "adc.h"

enum class SystemStatus_t {
    Waiting,
    Watering,
    EmptyContainer,
    Error
};

volatile uint32_t RawHumValue = 0;



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
    int GetDesiredHumidity() const { // Getter does not change the parameter (const-correctness)
        return DesiredHumidity;
    }
    // b) Getter for the current system status
    SystemStatus_t GetSystemStatus() const { // Getter does not change the parameter (const-correctness)
        return SystemStatus;
    }
    // *******************


    // System status update logic 
    void UpdateSystem(uint8_t currentHumidity, bool WaterLevel_OK) {

        if (WaterLevel_OK) {
        if (currentHumidity > 100) {
                SystemStatus = SystemStatus_t::Error;
            }
        else if (currentHumidity <= DesiredHumidity) {
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
};


Pot AloePot(40);


void app_main() {




while (1) {
  

}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
if (htim->Instance == TIM6) {
HAL_ADC_Start_IT(&hadc1);
}

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) {
        RawHumValue = HAL_ADC_GetValue(hadc);
    }
}
  

