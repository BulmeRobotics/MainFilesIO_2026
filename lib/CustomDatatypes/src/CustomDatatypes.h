#pragma once


#include <cstdint>

enum class RobotState : uint8_t {
    BOOT, INFO_SENSOR, INFO_VISUAL, SETTINGS, ABOUT, CALIBRATION, RUN, BT, CHECKPOINT
};

enum class ErrorCodes : uint8_t {
    OK,
    ERROR,
    UNKNOWN,
    TIMEOUT,
    NEW_DATA,
    NO_NEW_DATA,
    OUT_OF_RANGE,
};

enum class TofType : uint8_t {
    LEFT_FRONT,
    LEFT_BACK,
    RIGHT_FRONT,
    RIGHT_BACK,
    FRONT,
    BACK,
};

enum class TofStatus : uint8_t{
    VALID,
    OUT_OF_RANGE,
    TIMEOUT,
    ERROR
};

struct GyroData {
	float angle_abs;
	float angle_car;
	float angle_error;
	bool direction_left;
	bool direction_right;
};

enum class GyroAxles : uint8_t {
	Axis_X,
	Axis_Y,
	Axis_Z
};

enum class Orientations : uint8_t {
	North, East, South, West
};

enum class PoI_Type : uint8_t {
    undef = 0,
    white, blue, checkpoint, black, dangerZone,
    harmed, safe, unharmed,
    red, yellow, green
};