#pragma once
/**
* @name:    Ejector.h
* @date:	16.01.2026
* @authors: Florian Wiesner, Paul Charusa
* @details: Header file for the ejector class
*/

#ifdef _MSC_VER
    #pragma region Libraries //--------------------------------------------------------------------------------------------------
#endif
#include <Arduino.h>
#include <Servo.h>
#include "CustomDatatypes.h"

#ifdef _MSC_VER
    #pragma region Ejector Class //----------------------------------------------------------------------------------------------
#endif
class Ejector {
    public:
        // Constructor
        Ejector() = default;
        
        // Methods
        /**
        * @brief  Method to initialize and configure the Ejectors.
        */
        void Init(void);

        /**
        * @brief  Method to eject a certain amount of rescue kits with either the left or right Ejector.
        * @param  side bool to specify the side (EJECT_LEFT, EJECT_RIGHT). left is equal to false, right equal to true.
        * @param amount the number of rescure kits that will be ejected (1-5).
        * @return OK if no errors occured.
        *         UNKNOWN if a parameter was wrong.
        */
        ErrorCodes Eject(bool side, uint8_t amount);

        // Definitions
        #define EJECT_LEFT false
        #define EJECT_RIGHT true

    private:
        // Pins
        #define PIN_SERVO_LEFT 4
        #define PIN_SERVO_RIGHT 7

        // Positions
        #define POS_CLOSED_LEFT 80
        #define POS_OPEN_LEFT 0
        #define POS_CLOSED_RIGHT 100
        #define POS_OPEN_RIGHT 180

        // Timing
        #define DELAY_OPEN 500
        #define DELAY_CLOSE 250

        // Objects
        Servo servoLeft;
        Servo servoRight;
    };