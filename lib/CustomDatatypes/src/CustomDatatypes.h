#pragma once


#include <cstdint>

enum class RobotState : uint8_t {
    BOOT, INFO_SENSOR, INFO_VISUAL, SETTINGS, ABOUT, CALIBRATION, RUN, BT, CHECKPOINT
};

enum class ErrorCodes : uint8_t {
    OK,
    ERROR,
    UNKNOWN,
    INVALID,
    TIMEOUT,
    NEW_DATA,
    NO_NEW_DATA,
    OUT_OF_RANGE,
    BUMPER_WALL,
    BUMPER_DISABLED,
    RAMP_END,
    TURNED,
    NOT_ALIGNING,
    CHECK_DRIVE,
    SCAN_DRIVE,
    CHECK_RAMP,
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

enum class ReferenceObj : uint8_t {
    FRONT,
    BACK,
    ENCODER
};

struct TOF_Optimal_Value {
	uint16_t front;
	uint16_t back;
	ReferenceObj type;
};

struct PID_Coefficients {
	float P;
	float I;
	float D;
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