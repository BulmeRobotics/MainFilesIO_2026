#pragma once

/**
 * @author  Vincent Rohkamm
 * @brief   camera class for communication between Camera (either RASPI or OPENMV) and uC
 * @date    01.04.2026
 */

#include <CustomDatatypes.h>
#include <Ejector.h>

class Vcameras
{
private:
    /* data */

    bool _LeftEnabled, _RightEnabled;
    bool _LeftAlert, _RightAlert;
public:
    Vcameras(/* args */);
    ~Vcameras();


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
