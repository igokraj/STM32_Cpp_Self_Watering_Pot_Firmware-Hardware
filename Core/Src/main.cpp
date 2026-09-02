#include "main.hpp"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h" // It is not necessary in C++
#include "main.h" 

enum class SystemStatus_t {
    Waiting,
    Watering,
    EmptyContainer,
    Error
};


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