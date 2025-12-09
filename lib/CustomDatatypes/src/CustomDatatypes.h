#pragma once


#include <cstdint>

enum class RobotState : uint8_t {
    BOOT, INFO_SENSOR, INFO_VISUAL, SETTINGS, CALIBRATION, RUN, BT, CHECKPOINT
};

enum class ErrorCodes : uint8_t {
    OK,
    ERROR,
    UNKNOWN,
    TIMEOUT,
    NEW_DATA,
    OUT_OF_RANGE,
};

enum class TofType : uint8_t {
    SHORT_LEFT_FRONT,
    SHORT_LEFT_BACK,
    SHORT_RIGHT_FRONT,
    SHORT_RIGHT_BACK,
    MID_FRONT,
    MID_BACK,
};

enum class TofStatus : uint8_t{
    VALID,
    OUT_OF_RANGE,
    TIMEOUT,
};