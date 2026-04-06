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
    static constexpr UART* CAMERA_L = &Serial2;     // D18  TX1 PD_5
    static constexpr UART* CAMERA_R = &Serial3;  // D19  RX1 PD_6

    static constexpr uint8_t CAMERAL_PIN_INT = 40;
    static constexpr uint8_t CAMERAL_PIN_RST = 42;
    
    static constexpr uint8_t CAMERAR_PIN_INT = 41;
    static constexpr uint8_t CAMERAR_PIN_RST = 43;

    static constexpr uint32_t CAM_TIMEOUT = 5000;

    //Serial
    UART* _camL = CAMERA_L;
    UART* _camR = CAMERA_R;

    // --- related Objects ---
    Ejector* _ejector = nullptr;
    Driving* _robot = nullptr;
    Mapping* _mapper = nullptr;
    UserInterface* _ui = nullptr;

    Stream* _debug_ifc = nullptr;

    //Reset after detection
    bool _victimFound = false;
    uint32_t _timeFound;
    static constexpr uint32_t DEACT_TIME_VICTIM = 2000;
    ErrorCodes HandleReset();

    // --- Interface ---
    bool _connectedL = false;
    bool _connectedR = false;

    // --- State Fields ---
    bool _LeftEnabled = false, _RightEnabled = false;
    bool _LeftAlert = false, _RightAlert = false;
    bool _oldRed = false;

    // --- Response ---
    static volatile char _buffL[10];
    static volatile char _buffR[10];

    /**
     * @brief Recieves Commandos
     * @param side left / right
     * @param waittime time to block in ms
     * @return Commandostring
     */
    String Recieve(ErrorCodes side, uint32_t waittime = 0);

public:

    Vcameras(Stream* debugPort = nullptr) : _debug_ifc(debugPort) {}

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
