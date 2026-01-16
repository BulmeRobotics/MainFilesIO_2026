#pragma once
/**
* @name:    Motor.h
* @date:	16.01.2026
* @authors: Florian Wiesner
* @details: Header file for Motor driver class
*/

#ifdef _MSC_VER
    #pragma region Libraries //--------------------------------------------------------------------------------------------------
#endif
#include <Arduino.h>
#include <cmath>
#include <Servo.h>
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
        Motor(uint8_t pwmPin, uint8_t driver, float maxMotorVoltage, uint8_t driverPinCW, uint8_t driverPinCCW, uint8_t encoderPhaseA);

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
        void ResetEncoder(uint64_t value);

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
#endif