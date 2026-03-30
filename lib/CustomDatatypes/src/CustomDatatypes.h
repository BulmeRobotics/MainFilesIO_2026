#pragma once


#include <cstdint>

enum class RobotState : uint8_t {
    BOOT, INFO_SENSOR, INFO_VISUAL, SETTINGS, ABOUT, CALIBRATION, RUN, BT, CHECKPOINT
};

enum class RunState : uint8_t {
    INITIAL, SETTILE, GET_INSTRUCTIONS, CHECKPOINT_RESET, END, TURN, ALIGN, DRIVE, 
};

enum class ErrorCodes : uint8_t {
    OK, ERROR, UNKNOWN,
    TIMEOUT,
    NEW_DATA,
    OUT_OF_RANGE,
    invalid, wall, straight, north, east, south, west, Overflow //Mapping specific
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

enum class TileType : uint8_t {
    inactive = 0,
    unexplored,
    visited, obstacle,
    checkpoint, dangerZone, blue, black
};