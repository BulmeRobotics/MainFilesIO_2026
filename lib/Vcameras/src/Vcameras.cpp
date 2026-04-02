#include "Vcameras.h"
#include <UserInterface.h>

//---------------------------------------------------------------------------------------------------------
// Initialization
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Init(Ejector* ejector, Mapping* mapper, Driving* robot, UserInterface* ui){
    _ejector = ejector;
    _mapper = mapper;
    _robot = robot;
    _ui = ui;

    _ifc->print("<I>");
    if(Recieve() == ErrorCodes::OK){
        if(_response.indexOf("OK") == -1) return ErrorCodes::ERROR;
    }
    return ErrorCodes::OK;  
}

//---------------------------------------------------------------------------------------------------------
// Recieve UART
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Recieve(uint16_t timeout){
    uint32_t start = millis();
    String buffer;

    // Read until timeout or until we have at least one complete token <...>
    while (millis() - start <= timeout) {
        while (_ifc->available()) {
            int c = _ifc->read();
            if (c < 0) break;
            buffer += (char)c;
        }

        int s = buffer.indexOf('<');
        int e = (s >= 0) ? buffer.indexOf('>', s) : -1;
        if (s >= 0 && e > s) break; // have a complete token
    }

    if (buffer.isEmpty()) {
        _ui->ShowPopup("Camera timeout!", ErrorCodes::ERROR);
        return ErrorCodes::TIMEOUT;
    }

    int s = buffer.indexOf('<');
    int e = (s >= 0) ? buffer.indexOf('>', s) : -1;
    if (s >= 0 && e > s) {
        _response = buffer.substring(s + 1, e);
        return ErrorCodes::OK;
    }
    _ui->ShowPopup("response invalid!", ErrorCodes::warning);
    return ErrorCodes::invalid;
}

//---------------------------------------------------------------------------------------------------------
// Enable
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Enable(bool en, ErrorCodes side){
    char cSide = (side == ErrorCodes::left) ? 'L' : 'R';

    if(en){ //Enable
        _ifc->print("<E" + cSide + '>');
        if(Recieve() == ErrorCodes::OK){
            if(_response.indexOf("OK") == -1) return ErrorCodes::ERROR;

            if(side == ErrorCodes::left) _LeftEnabled = true;
            else _RightEnabled = true;
            return ErrorCodes::OK;
        }
    } else {    //Disable
        _ifc->print("<D" + cSide + '>');
        if(Recieve() == ErrorCodes::OK){
            if(_response.indexOf("OK") == -1) return ErrorCodes::ERROR;

            if(side == ErrorCodes::left) _LeftEnabled = false;
            else _RightEnabled = false; 
            return ErrorCodes::OK;
        }
    }
    _ui->ShowPopup("cams enable error", ErrorCodes::ERROR);
    return ErrorCodes::invalid;
}

//---------------------------------------------------------------------------------------------------------
// Enable
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Handler(){
    return ErrorCodes::OK;
}