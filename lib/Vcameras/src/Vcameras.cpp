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

    _camL->print("<I>");
    if(Recieve(_camL) == ErrorCodes::OK){
        if(_response.indexOf("OK") == -1) return ErrorCodes::ERROR;
    }

    _camR->print("<I>");
    if(Recieve(_camR) == ErrorCodes::OK){
        if(_response.indexOf("OK") == -1) return ErrorCodes::ERROR;
    }
    return ErrorCodes::OK;  
}

//---------------------------------------------------------------------------------------------------------
// Recieve UART
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Recieve(Stream* ifc, uint16_t timeout){
    uint32_t start = millis();
    String buffer;

    // Read until timeout or until we have at least one complete token <...>
    while (millis() - start <= timeout) {
        while (ifc->available()) {
            int c = ifc->read();
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
    Stream* ifc = (side == ErrorCodes::left) ? _camL : _camR;

    if(en){ //Enable
        ifc->print("<E>");
        if(Recieve(ifc) == ErrorCodes::OK){
            if(_response.indexOf("OK") == -1) return ErrorCodes::ERROR;

            if(side == ErrorCodes::left) _LeftEnabled = true;
            else _RightEnabled = true;
            return ErrorCodes::OK;
        }
    } else {    //Disable
        ifc->print("<D>");
        if(Recieve(ifc) == ErrorCodes::OK){
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

}