#pragma once

// @author  Vincent Rohkamm
// @date     07.07.2025
// @brief Header file for Bluetooth functionality for ROBOT

#ifdef _MSC_VER
#pragma region Includes
#endif

//Standard Libraries
#include <Arduino.h>
#include <ArduinoBLE.h>

//Custom Libraries
#include "CustomDatatypes.h"
#include "DisplayUI.h"


#ifdef _MSC_VER
#pragma endregion Includes
#endif

class BT_commObj{
    private:
        uint16_t timeoutTime = 0;
        bool _ENABLE_DEBUG = false;
        bool _ENABLE_BLE = true;

        BLEDevice peripheral;
        BLECharacteristic rxCharacteristic;
        BLECharacteristic txCharacteristic;
        const String serviceUUID    = "FF000000-FFFF-EEEE-99FF-123456789000"; //Service UUID for ESP32-INFO
        #define rxUUID              "FF000000-FFFF-EEEE-99FF-123456789002" //RX UUID for ESP32-INFO
        #define txUUID              "FF000000-FFFF-EEEE-99FF-123456789001" //TX UUID for ESP32-INFO

        Robot_UI* p_UI = nullptr;
    public:
        //Constructors--------------------------------------------------------------------------------
        BT_commObj(uint16_t timeoutTime, bool enableDEBUG, Robot_UI* UI, bool enableBLE){
            this->p_UI = UI;
            this->timeoutTime = timeoutTime;
            this->_ENABLE_DEBUG = enableDEBUG;
            this->_ENABLE_BLE = enableBLE;
        }
        BT_commObj(uint16_t timeoutTime, bool enableDEBUG, Robot_UI* UI){
            this->p_UI = UI;
            this->timeoutTime = timeoutTime;
            this->_ENABLE_DEBUG = enableDEBUG;
            this->_ENABLE_BLE = true;
        }
        BT_commObj(uint16_t timeoutTime, bool enableDEBUG){
            this->timeoutTime = timeoutTime;
            this->_ENABLE_DEBUG = enableDEBUG;
            this->_ENABLE_BLE = true;
        }
        BT_commObj(void){
            this->timeoutTime = 5000; //Default timeout time
            this->_ENABLE_DEBUG = false; //Default debug state = OFF
            this->_ENABLE_BLE = true;
        }

        //Methods--------------------------------------------------------------------------------

        //@brief Initialize the Bluetooth LowEnergy communication
        //@return ErrorCodes::OK...success, ErrorCodes::I2CError...failed to init
        ErrorCodes init(void);

        //@brief connect to a BLE peripheral, waits till connected or timeout
        ErrorCodes connect(void);

        //@brief write data to Peripheral
        //@param data...data to write (uint8_t)
        ErrorCodes writeData(uint8_t data);

        uint8_t readData(void);

        //@brief check ENABLE status of BLE
        ErrorCodes checkEN(void);
};