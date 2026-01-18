#pragma once
/**
* @name:    Motor.h
* @date:	16.01.2026
* @authors: Florian Wiesner
* @details: Header file for Motor driver class and the Drivetrain class
*/

#ifdef _MSC_VER
    #pragma region Libraries //--------------------------------------------------------------------------------------------------
#endif
#include <Arduino.h>
#include <cmath>
#include "CustomDatatypes.h"

//Encoder ISR - outside class
void encoder_ISR(void);

#ifdef _MSC_VER
    #pragma endregion
    #pragma region Motor Class //------------------------------------------------------------------------------------------------
#endif
class Motor {
    public:
        // Configuration
        #define WHEEL_DIAMETER 93.0
        #define MAX_VOLTAGE_PWM 8.26
        #define GEAR_REDUCTION_RATIO 74.83
        #define CLICKS_PER_MOTOR_ROTATION 11
        #define CLICKS_PER_SHAFT_ROTATION GEAR_REDUCTION_RATIO * CLICKS_PER_MOTOR_ROTATION
    
        // Definitions
        #define DRIVER_A 0
        #define DRIVER_B 1

        // Constructor
        /**
        * @brief  Constructor of the motor driver class.
        * @param  pwmPin PWM pin that controls the motor
        * @param  driver DRIVER_A or DRIVER_B, depending to which the motor is connected
        * @param  maxMotorVoltage from this voltage the maximum PWM-duty-cycle will be calculated (max. 8.26V).
        * @param  driverPinCW the pin that connect to the clockwise direction-pin of the driver
        * @param  driverPinCCW the pin that connect to the counter-clockwise direction-pin of the driver
        * @param  encoderPhaseA one of the two encoder pins of the motor.
        */
        Motor(uint8_t pwmPin, uint8_t driver, uint8_t driverPinCW, uint8_t driverPinCCW, uint8_t encoderPhaseA, float maxMotorVoltage = 8.26);

        // Methods
        /**
        * @brief  Method to set the speed of the motor in percent.
        * @param  speed percentage of the speed (-100% to +100%).
        */
        void SetSpeed(int8_t speed);

        /**
        * @brief  Method to stop the motor.
        */
        void Stop(void);

        /**
        * @brief  Method to reset the encoder to a value(to mostly 0).
        * @param  value new number of the counter (default = 0).
        */
        void ResetEncoder(uint64_t value = 0);

        /**
        * @brief  Method to get the current angle of the motor shaft.
        * @return current value of the motor angle.
        */
        float GetEncoderAngle(void);

        /**
        * @brief  Method to get the distance the wheel has driven since the last reset.
        * @return travelled distance of the wheel (in mm).
        */
        float GetEncoderDistance(void);

        /**
        * @brief  Method to enable the encoder. Enables the interrupt.
        */
        void EnableEncoder(void);

        /**
        * @brief  Method to disable the encoder. Disables the interrupt.
        */
        void DisableEncoder(void);

    private:
        // Member
        uint8_t cwPinA, ccwPinA, cwPinB, ccwPinB, pwmPin, encoderPhaseA;
        uint8_t driver;
        float maxMotorVoltage;
        int16_t maxPWM;

        // Methods
        void SetPositiveDirection(void);
        void SetNegativeDirection(void);
        void InitDriverPins(uint8_t pinA, uint8_t pinB);
};


#ifdef _MSC_VER
    #pragma endregion
    #pragma region Drivetrain Class //------------------------------------------------------------------------------------------------
#endif
class Drivetrain {
    public:
        // Configuration
        #define MOTOR_LB_PWM 13
        #define MOTOR_LB_CW 36
        #define MOTOR_LB_CCW 39
        #define MOTOR_LB_ENCODER 5
        #define MOTOR_LB_DRIVER DRIVER_A
        
        #define MOTOR_LF_PWM 11
        #define MOTOR_LF_CW 36
        #define MOTOR_LF_CCW 39
        #define MOTOR_LF_ENCODER 6
        #define MOTOR_LF_DRIVER DRIVER_A

        #define MOTOR_RF_PWM 10
        #define MOTOR_RF_CW 37
        #define MOTOR_RF_CCW 38
        #define MOTOR_RF_ENCODER 3
        #define MOTOR_RF_DRIVER DRIVER_B

        #define MOTOR_RB_PWM 9
        #define MOTOR_RB_CW 37
        #define MOTOR_RB_CCW 38
        #define MOTOR_RB_ENCODER 2
        #define MOTOR_RB_DRIVER DRIVER_B

        // Constructor
        Drivetrain() = default;

        // Methods
        /**
        * @brief  Overloaded method to set the speed of the motors in percent.
        *         When 1 parameter is given, the value applies to all 4 motors.
        *         When 4 parameters are given, the values apply to the corresponding motor.
        * @param  speedLB Motor-Left-Back: percentage of the speed (-100% to +100%).
        * @param  speedLF Motor-Left-Front: percentage of the speed (-100% to +100%).
        * @param  speedRF Motor-Right-Front: percentage of the speed (-100% to +100%).
        * @param  speedRB Motor-Right-Back: percentage of the speed (-100% to +100%).
        * @param  speed For all: percentage of the speed (-100% to +100%).
        */
        void SetSpeed(int8_t speedLB, int8_t speedLF, int8_t speedRF, int8_t speedRB);
        void SetSpeed(int8_t speed);

        /**
        * @brief  Method to set the speed of the left-back motor in percent.
        * @param  speed percentage of the speed (-100% to +100%).
        */
        void SetSpeed_LB(int8_t speed);

        /**
        * @brief  Method to set the speed of the left-front motor in percent.
        * @param  speed percentage of the speed (-100% to +100%).
        */
        void SetSpeed_LF(int8_t speed);

        /**
        * @brief  Method to set the speed of the right-front motor in percent.
        * @param  speed percentage of the speed (-100% to +100%).
        */
        void SetSpeed_RF(int8_t speed);

        /**
        * @brief  Method to set the speed of the right-back motor in percent.
        * @param  speed percentage of the speed (-100% to +100%).
        */
        void SetSpeed_RB(int8_t speed);

        /**
        * @brief  Method to set the speed of the left-sided motors in percent.
        * @param  speed percentage of the speed (-100% to +100%).
        */
        void SetSpeed_Left(int8_t speed);

        /**
        * @brief  Method to set the speed of the right-sided motors in percent.
        * @param  speed percentage of the speed (-100% to +100%).
        */
        void SetSpeed_Right(int8_t speed);
        
        /**
        * @brief  Method to stop all motors.
        */
        void Stop(void);

        /**
        * @brief  Method to reset the encoder to a value(to mostly 0).
        * @param  value new number of the counter (default = 0).
        */
        void ResetEncoder(uint64_t value = 0);


        /**
        * @brief  Method to get the distance the robot has driven since the last reset. Uses only Left-Back!
        * @return travelled distance of the robot (in mm).
        */
        float GetEncoderDistance(void);

        /**
        * @brief  Method to enable the encoder. Enables the interrupt (only Left-Back).
        */
        void EnableEncoder(void);

        /**
        * @brief  Method to disable the encoder. Disables the interrupt (only Left-Back).
        */
        void DisableEncoder(void);

    private:
        // Member
        Motor motorLB = Motor(MOTOR_LB_PWM, MOTOR_LB_DRIVER, MOTOR_LB_CW, MOTOR_LB_CCW, MOTOR_LB_ENCODER);
        Motor motorLF = Motor(MOTOR_LF_PWM, MOTOR_LF_DRIVER, MOTOR_LF_CW, MOTOR_LF_CCW, MOTOR_LF_ENCODER);
        Motor motorRF = Motor(MOTOR_RF_PWM, MOTOR_RF_DRIVER, MOTOR_RF_CW, MOTOR_RF_CCW, MOTOR_RF_ENCODER);
        Motor motorRB = Motor(MOTOR_RB_PWM, MOTOR_RB_DRIVER, MOTOR_RB_CW, MOTOR_RB_CCW, MOTOR_RB_ENCODER);
};
#ifdef _MSC_VER
    #pragma endregion
#endif