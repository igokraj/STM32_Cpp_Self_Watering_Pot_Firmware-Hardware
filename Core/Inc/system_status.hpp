#pragma once

// Shared between the control logic (Pot) and the OLED UI
enum class SystemStatus_t {
    Waiting,
    Watering,
    EmptyContainer,
    Error
};
