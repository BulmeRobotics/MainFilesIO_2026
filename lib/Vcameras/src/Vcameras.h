#pragma once

/**
 * @author  Vincent Rohkamm
 * @brief   camera class for communication between Camera (either RASPI or OPENMV) and uC
 * @date    01.04.2026
 */

#include <Arduino.h>
#include <mbed.h>

#include <CustomDatatypes.h>
#include <Ejector.h>
#include <Driving.h>
#include <Mapping.h>

class UserInterface;

class Vcameras
{
private:
    // --- Hardware Info ---
    static constexpr PinName CAMERAL_TX = PD_5;     // D18  TX1 PD_5
    static constexpr PinName CAMERAL_RX = PD_6;     // D19  RX1 PD_6
    static constexpr PinName CAMERAR_TX = PH_13;    // D16  TX2 PH_13
    static constexpr PinName CAMERAR_RX = PI_9;     // D17  RX2 PI_9

    static constexpr uint8_t CAMERAL_PIN_INT = 40;
    static constexpr uint8_t CAMERAL_PIN_RST = 42;
    
    static constexpr uint8_t CAMERAR_PIN_INT = 41;
    static constexpr uint8_t CAMERAR_PIN_RST = 43;

    //Serial
    static mbed::UnbufferedSerial _camL;
    static mbed::UnbufferedSerial _camR;

    // --- related Objects ---
    Ejector* _ejector = nullptr;
    Driving* _robot = nullptr;
    Mapping* _mapper = nullptr;
    UserInterface* _ui = nullptr;

    // --- Interface ---
    bool _connectedL = false;
    bool _connectedR = false;

    // --- State Fields ---
    bool _LeftEnabled = false, _RightEnabled = false;
    bool _LeftAlert = false, _RightAlert = false;
    bool _oldRed = false;

    // --- Response ---
    static char _buffL[7];
    static char _buffR[7];
    static uint8_t _idL;
    static uint8_t _idR;

    static bool _NEW_DATA_L;
    static bool _NEW_DATA_R;

    static void on_camL_int();
    static void on_camR_int();

    /**
     * @brief Recieves Commandos
     * @param side left / right
     * @param waittime time to block in ms
     * @return Commandostring
     */
    String Recieve(ErrorCodes side, uint32_t waittime = 0);

public:

    /**
     * @brief Initializes Cam class. Tries connecting to cameras
     * @return OK - success, else ErrorCodes
     */
    ErrorCodes Init(Ejector* ejector, Mapping* mapper, Driving* robot, UserInterface* ui);

    /**
     * @brief enables or disables Camera
     * @param en true...on, false...off
     * @param side left / right
     * @return OK / Error
     */
    ErrorCodes Enable(bool en, ErrorCodes side);

    /**
     * @brief camera handler has to be called periodically
     * @param onRed is robot on Red Tile?
     * @param wallL is a wall Left?
     * @param wallR is a wall Right?
     * @return ErrorCodes for debugging
     */
    ErrorCodes Update(bool onRed, bool wallL = true, bool wallR = true);

    /**
     * @brief Getter if Cam is enabled
     * @param cam ErrorCodes::left / ErrorCodes::right
     * @return true...enabled, false...disabled
     */
    bool IsEnabled(ErrorCodes cam){
        return (cam == ErrorCodes::left) ? _LeftEnabled : _RightEnabled;
    }

    /**
     * @brief Getter if Cam is in Alert
     * @param cam ErrorCodes::left / ErrorCodes::right
     * @return true...enabled, false...disabled
     */
    bool IsAlert(ErrorCodes cam){
        return (cam == ErrorCodes::left) ? _LeftAlert : _RightAlert;
    }
};
