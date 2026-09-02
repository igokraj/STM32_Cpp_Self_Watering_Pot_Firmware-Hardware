#include "main.hpp"
#include "main.h"
#include "stm32f4xx_hal.h"

enum class SystemStatus_t {
    Waiting,
    Watering,
    EmptyContainer,
    Error
};


class Pot {

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

    // ***** SETTER *****
    void SetDesiredHumidity(uint8_t DesiredHumidity) {
        this->DesiredHumidity = DesiredHumidity;
    }



};




void app_main() {





while (1) {
    

}
}