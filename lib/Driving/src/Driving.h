#pragma once

//Created:	21.04.2024
//Copied:   30.03.2026
//Authors:	Vincent Rohkamm, Florian Wiesner

#include <Arduino.h>
#include <math.h>

#include <CustomDatatypes.h>
#include <Motor.h>
#include <TofSensors.h>
#include <Gyro.h>
#include <ColorSensing.h>
#include <Mapping.h>
// #include <Mapping.h>

class Cameras; // Forward declaration of the Cameras class

class Driving {
protected:

    //#define DEBUG_RAMP
    //#define DEBUG_RAMP_ARRAY
    //#define DEBUG_X64


private:
//CONFIGURATION -------------------------------------------------------------------------------------------------------------------------
    #define MIN_SETTILE_TIME 750
    #define INCLINE_ARRAY_SIZE 250
    #define reverseBumperTimeout 2000

    //Driving consants
    const uint8_t tof_sideWallThreshold = 170;
    const uint8_t gap_robot_wall = 75; //Distance between the robot and the wall
    const uint8_t stop_wallDistance = 75;
    const uint8_t adjust_wallDistance = 70;
    const uint8_t adjustmentSpeedFactor = 1.5;	//Speed factor for the adjustment speed

    //Ramp settings
    const float rampThresholdAngle = 10;	//Angle threshold for the ramp
    const float rampConfidence = 0.9;    //Confidence for the ramp detection
    const uint8_t rampSpeedUp = 75;
    const uint8_t rampSpeedDown = 25;
    const uint8_t rampSubDistance = 30;
    const float rampStairsThreshold = 60;
    const float rampUp_K = 0.95;
    const float rampUp_d = 30; 
    const float rampDown_K = 1.7;
    const float rampDown_d = 25;
    const float stairUp_K = 0.7;
    const float stairUp_d = 0;
    const float stairDown_K = 1.15;
    const float stairDown_d = 0;	
    const float stairUp_angle_offset = 3.5;
    const float stairDown_angle_offset = -3.5;

    //PID coefficents
    //const float pidCoeff_P
    const float pid_CriticalGain = 7.5;			//Maximum proportional amount for P-Controller
    const float pid_LoopDuration = 0.025;	//Duration of the control loop
    const float pid_OscillationPeriod = 0.75;	//Period time of the oscillation of the robot
    const float pid_LeftRightFactor = 0.35;

    const uint8_t BUMPER_TRYS = 5;
    const uint8_t BUMPER_LEFT_PIN = 47;
    const uint8_t BUMPER_RIGHT_PIN = 45;

//Obj Pointer -------------------------------------------------------------------------------------------------------------------------
    ColorSensing*  p_colorSensing = nullptr;
    TofSensors*    p_tof = nullptr;
    Gyro*          p_gyro = nullptr;
    Drivetrain*    p_drivetrain = nullptr;
    Mapping*       p_mapSys = nullptr;
    // Cameras*       p_cams = nullptr;

//Objects -------------------------------------------------------------------------------------------------------------------------
    

//Private Variables -------------------------------------------------------------------------------------------------------------------------
    //FLAGS
    //Bumper
    bool    _ENABLE_BUMPERS = true;
    uint8_t _registeredBumps = 0;

    //Ramps
    ErrorCodes finishRamp(uint8_t distance);
    ErrorCodes checkRamp(void);
    
    TOF_Optimal_Value 	lastSensor;
    uint32_t 			startTime;
    bool 				turnTimeout;

    uint16_t 			lastTargetDistance;
    uint8_t 			partsDriven;
    uint16_t 			encoderDistance;
    String 				side;
    int8_t 				coeff_side;
    uint8_t 			distanceFront;
    uint8_t 			distanceBack;
    uint32_t            encoderStartTime;
    uint32_t            driveStartTime = 0;
    uint32_t            maxDriveTime = 5000;
    uint16_t            maxEncoderTime = 3000;

    int16_t 			distanceError;
    int16_t 			turnSpeed_align;
    uint16_t			maxTurnTime = 3000;
    int16_t 			inclineCycleCounter = 0;
    uint16_t 			nonInclineCycleCounter = 0;
    uint32_t 			rampStartTime = 0;
    uint16_t 			rampCheckDuration = 1000;
    uint8_t 			rampSpeed;
    float				rampEncoderDistance;
    float 				RAMP_HYPOTENUSE = 0.0;
    
    float 				RAMP_ANGLE = 0.0;
    float               avgIncline = 0.0;
    float 				arr_incline[INCLINE_ARRAY_SIZE] = { 0.0 };
    uint16_t 			arr_incline_index = 0;
    
    float 				pid_lastError;
    float 				correctionSpeed;
    long 				lastPID_timestamp = 0;
    
    bool				_RAMP_UP = false;
    bool				_RAMP_DOWN = false;
    bool                _STAIR = false;
    bool 				_TURN_180_DEGREE = false;
    bool                _DRIVE_TIMEOUT = false;
    bool 	            _CAM_ALERT_TURN = false;
    
    //speedMOD
    const uint8_t stdSPEED_MOD = 2;
    const uint8_t slowSPEED_MOD = 4;
    

    //General
    PID_Coefficients calculatePIDCoefficients(float);
    TOF_Optimal_Value getOptimalSensor(void);

public:
    //PUBLIC FLAGS
    bool	_ON_RAMP = false;
    bool 	_RAMP_BEHIND = false;
    bool	_RAMP_INFRONT = false;
    bool 	_RAMP_INSTRUCTION = false;

    //Public Variables
    TOF_Optimal_Value 	sensor;
    uint16_t 			nextTargetDistance;
    uint16_t 			newValue;   //Actual distance to Target
    uint8_t 			speedMOD = stdSPEED_MOD;
    Orientations        robotTargetAngle = Orientations::North;

    //PID
    float 		integralError;
    float 		derivativeError;
    
    //RAMP
    float 		maxRampIncline;
    float 		RAMP_HEIGHT = 0.0;
    float 		RAMP_LENGTH = 0.0;
    int16_t		currentRobotHeight;
    uint32_t    lastSetTile;

    //@brief initializes driving
    void init(ColorSensing* p_colorSensing, TofSensors* p_tof, Gyro* p_gyro, Mapping* mapSys_pointer, /*Cameras* cam_pointer, */ Drivetrain* p_drivetrain);

    //Bumpers:
    ErrorCodes reverseBumper(uint16_t distance, int8_t speedLeft, int8_t speedRight);
    ErrorCodes enableBumpers(void);
    ErrorCodes disableBumpers(void);
    ErrorCodes bumperHandler(void);

    //Ramps:
    ErrorCodes rampHandler(void);
    bool checkStairRamp(void);

    //Turn

    ErrorCodes startTurn(float angle);
    ErrorCodes controlTurn(float angle);
    ErrorCodes endTurn();
    ErrorCodes turn180Degree(void);
    ErrorCodes startAlign(void);

    //Drive
    ErrorCodes startDrive(void);
    ErrorCodes controlDrive(int8_t drivespeed, float angle);
    ErrorCodes checkDrive(void);
    ErrorCodes endDrive(void);
    ErrorCodes timeoutDrive(void);
    ErrorCodes startAdjustment(void);
    ErrorCodes reverseBlackTile(void);
};

