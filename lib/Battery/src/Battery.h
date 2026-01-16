#pragma once
/**
* @name:    Battery.h
* @date:	15.01.2026
* @authors: Florian Wiesner, Paul Charusa
* @details: Header file for Battery Class
*/

#ifdef _MSC_VER
    #pragma region Libraries //--------------------------------------------------------------------------------------------------
#endif
#include <Arduino.h>
#include <UserInterface.h>
#include "CustomDatatypes.h"

#ifdef _MSC_VER
    #pragma endregion
    #pragma region Battery Class //----------------------------------------------------------------------------------------------
#endif
class Battery {
    public:
        // Constructor
        Battery(uint32_t updateIntervalTime, UserInterface* pointerUI) {
            intervalTime = updateIntervalTime;
            lastUpdate = millis();
            p_UI = pointerUI;
        }
        Battery(){
            intervalTime = 5000; // Default to 5 seconds
            lastUpdate = millis();
        }
    
        // Methods
        ErrorCodes BatteryUpdateHandler();
        ErrorCodes InitScreen();

        /**
        * @brief  Method to get the volatage level of the battery.
        * @return battery voltage in V.
        */
        float GetBatteryVoltage(void);

        /**
        * @brief  Method to get the volatage level of the battery in percent (0-100%).
        * @return volatage level percent (0-100%).
        */
        uint8_t GetBatteryPercent(void);
    
    private:
        // Member
        int last_per;
        uint32_t intervalTime = 0;
        uint32_t lastUpdate = 0;
        UserInterface* p_UI = nullptr;
        
};
#ifdef _MSC_VER
    #pragma endregion
#endif