#pragma once


#include <cstdint>

enum class RobotState : uint8_t {
    BOOT, INFO_SENSOR, INFO_VISUAL, SETTINGS, ABOUT, CALIBRATION, RUN, BT, CHECKPOINT
};

enum class RunState : uint8_t {
    INITIAL, SETTILE, GET_INSTRUCTIONS, CHECKPOINT_RESET, END, TURN, ALIGN, DRIVE, 
};

enum class ErrorCodes : uint8_t {
<<<<<<< HEAD
    OK,
    ERROR,
    UNKNOWN,
    INVALID,
=======
    OK, ERROR, UNKNOWN,
>>>>>>> 37eae5f4e0bd41fcfccf5b08a225d3d34a03d391
    TIMEOUT,
    NEW_DATA,
    NO_NEW_DATA,
    OUT_OF_RANGE,
<<<<<<< HEAD
    BUMPER_WALL,
    BUMPER_DISABLED,
    RAMP_END,
    TURNED,
    NOT_ALIGNING,
    CHECK_DRIVE,
    SCAN_DRIVE,
    CHECK_RAMP,
=======
    invalid, wall, straight, north, east, south, west, Overflow //Mapping specific
>>>>>>> 37eae5f4e0bd41fcfccf5b08a225d3d34a03d391
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

enum class TileType : uint8_t {
    inactive = 0,
    unexplored,
    visited, obstacle,
    checkpoint, dangerZone, blue, black
};