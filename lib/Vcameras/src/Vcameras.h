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
    static constexpr uint8_t CAMERAL_TX = 01;
    static constexpr uint8_t CAMERAL_RX = 0;
    static constexpr uint8_t CAMERAR_TX = 01;
    static constexpr uint8_t CAMERAR_RX = 0;

    static constexpr uint8_t CAMERAL_PIN_INT = 40;
    static constexpr uint8_t CAMERAL_PIN_RST = 42;
    
    static constexpr uint8_t CAMERAR_PIN_INT = 41;
    static constexpr uint8_t CAMERAR_PIN_RST = 43;

    //Serial
    mbed::UnbufferedSerial _camL;
    mbed::UnbufferedSerial _camR;

    //(arduino::digitalPinToPinName(CAMERAL_TX), digitalPinToPinName(CAMERAL_RX));
    //(digitalPinToPinName(CAMERAR_TX), digitalPinToPinName(CAMERAR_RX));

    // --- related Objects ---
    Ejector* _ejector = nullptr;
    Driving* _robot = nullptr;
    Mapping* _mapper = nullptr;
    UserInterface* _ui = nullptr;

    // --- Interface ---
    Stream* _ifc = nullptr;
    bool _connected = false;

    // --- State Fields ---
    bool _LeftEnabled = false, _RightEnabled = false;
    bool _LeftAlert = false, _RightAlert = false;
    bool _oldRed = false;

    // --- Response ---
    String _response = "";

    // --- helper Methods
    ErrorCodes Recieve(uint16_t timeout = 200);
public:
    /**
     * @brief Constructor for VictimCamera class
     * @param camL ifc for left Cam
     * @param camR ifc for right Cam
     */
    Vcameras(Stream* ifc = &Serial3) : _ifc(ifc) {}

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
     * @return ErrorCodes for debugging
     */
    ErrorCodes Update(bool onRed);

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
