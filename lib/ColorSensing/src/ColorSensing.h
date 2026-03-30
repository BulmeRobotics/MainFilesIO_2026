#pragma once

// author: Vincent Rohkamm
// date: 18.11.2025
// description: User Interface for Robot

#ifdef _MSC_VER
    #pragma region Includes //-----------------------------------------------------------------------
#endif

#include <Arduino.h>
#include "CustomDatatypes.h"
#include <Adafruit_AS7341.h>
#include <Adafruit_EEPROM_I2C.h>
#include <Adafruit_FRAM_I2C.h>

#ifdef _MSC_VER
    #pragma endregion Includes //-----------------------------------------------------------------------
    #pragma region eeprom
#endif

class EEPROM {
    private:

#define EEPROM_ADDR 0x50  

#define EEPROM_PACKAGE_SIZE 2
#define EEPROM_PACKAGE_OVERHEAD 1
#define EEPROM_PACKAGE_NUM 8

#define EEPROM_START_ADD_FRONT_WHITE 0x000
#define EEPROM_START_ADD_MIDDLE_WHITE 0x019

#define EEPROM_START_ADD_FRONT_BLUE 0x032
#define EEPROM_START_ADD_MIDDLE_BLUE 0x04B

#define EEPROM_START_ADD_FRONT_CP 0x064
#define EEPROM_START_ADD_MIDDLE_CP 0x07D

#define EEPROM_START_ADD_FRONT_BLACK 0x096
#define EEPROM_START_ADD_MIDDLE_BlACK 0x0AF

#define EEPROM_START_ADD_FRONT_DZ 0x0C8
#define EEPROM_START_ADD_MIDDLE_DZ 0x0FA

#define EEPROM_START_ADD_REFLECTIVE 0x12C

#define EEPROM_MAIN_SPEED 0x12C

#define EEPROM_NEXT_AVALIBLE_ADDR 0x15E

    Adafruit_EEPROM_I2C i2ceeprom;

    int16_t GetStartAddr(PoI_Type type, char sensor);


    public:
        ErrorCodes Init();
        ErrorCodes WriteColorSens(PoI_Type type, char sensor, uint32_t* buffer);
        ErrorCodes ReadColorSens(PoI_Type type, char sensor, uint32_t* buffer);
};

#ifdef _MSC_VER
    #pragma endregion  eeprom //-----------------------------------------------------------------------
    #pragma region Color Sensing
#endif


class ColorSensing{
    private:
    #define MULTIPLEX_ADRESS 0x70
    //Objects for CS
        EEPROM eeprom;
        Adafruit_AS7341 Front;
	    Adafruit_AS7341 Middle;

        PoI_Type colorFront;
        PoI_Type colorMiddle;

        bool _ALERT;
        bool _FREEZE_SENSOR;

        /**
         * @brief toggles between multiplexer channels; enables use of multiple CS
         * @param bus: 0...front; 1...middle
         */
        void TCA9548A(uint8_t bus){
            Wire.beginTransmission(MULTIPLEX_ADRESS);    // TCA9548A address
            Wire.write(1 << bus);                   // send byte to select bus
            Wire.endTransmission();
        }

    public:

        /**
         * @brief inits ColorSensing for Floor
         * @param wire: Wire channel
         * @param addr_front: I2C address of front CS
         * @param addr_middle: I2C address of middle CS
         * @return	b1...Middle not found
		 * @return	b2...LED Error front
		 * @return	b3...LED Error middle
		 * @return	b4...Front GAIN Error
		 * @return	b5...Middle GAIN Error
		 * @return	b6...Middle ATIME / ASTEP Error
		 * @return	b7...Front ATIME / ASTEP Error
         */
        uint8_t Init(TwoWire* wire);

        /**
         * @brief returns current floor type
         * @return current floor type as PoI_Type
         */
        PoI_Type GetType();

        /**
         * @brief returns if the front sensor registers not White
         * @return _ALERT: true...not white; false...white
         */
        bool GetAlert();

        /**
         * @brief freezes the cs output to white, use on ramp, etc
         * @param enable: true...freeze; false...normal operation
         * @return ErrorCodes
         */
        ErrorCodes Freeze(bool enable);

        /**
         * @brief calibrates all color sensors to selected color
         * @param color: calibrated floor color
         * @return returns if successful
         */
        ErrorCodes Calibrate(PoI_Type color);

};