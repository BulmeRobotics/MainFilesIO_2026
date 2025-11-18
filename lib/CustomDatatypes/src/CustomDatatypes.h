#pragma once


#include <cstdint>

enum class RobotState : uint8_t {
    BOOT, INFO_SENSOR, INFO_VISUAL, SETTINGS, CALIBRATION, RUN, BT
};

enum class ErrorCodes : uint8_t {
    OK,
    UNKNOWN
};