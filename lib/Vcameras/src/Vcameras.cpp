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

    pinMode(CAMERAL_PIN_INT, INPUT);
    pinMode(CAMERAR_PIN_INT, INPUT);

    _camL->print("<I>");
    if(Recieve(_camL, true) != ErrorCodes::OK || _response.indexOf("OK") == -1){
        _connectedL = false;
        return ErrorCodes::ERROR;
    }
    _connectedL = true;
    
    _camR->print("<I>");
    if(Recieve(_camR, true) != ErrorCodes::OK || _response.indexOf("OK") == -1){
        _connectedR = false;
        return ErrorCodes::ERROR;
    }
    _connectedR = true;


    return ErrorCodes::OK;  
}

//---------------------------------------------------------------------------------------------------------
// Recieve UART
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Recieve(Stream* ifc, bool connected, uint16_t timeout){
    if(!connected) return ErrorCodes::no_connection;
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
    bool conn = (side == ErrorCodes::left) ? _connectedL : _connectedR;
    if(conn) return ErrorCodes::no_connection;
    Stream* ifc = (side == ErrorCodes::left) ? _camL : _camR;

    if(en){ //Enable
        ifc->print("<E>");
        if(Recieve(ifc,conn) == ErrorCodes::OK){
            if(_response.indexOf("OK") == -1) return ErrorCodes::ERROR;
            if(side == ErrorCodes::left) _LeftEnabled = true;
            else _RightEnabled = true;
            return ErrorCodes::OK;
        }
    } else {    //Disable
        ifc->print("<D>");
        if(Recieve(ifc, conn) == ErrorCodes::OK){
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
// Update
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Update(bool onRed){
    if(!_connectedL || !_connectedR) return ErrorCodes::no_connection;
    if(_oldRed && !onRed) {
        Enable(true, ErrorCodes::left);
        Enable(true, ErrorCodes::right);
    } else if (!_oldRed && onRed){
        Enable(false, ErrorCodes::left);
        Enable(false, ErrorCodes::right);
    }

    //Abfrage auf alert
    if(_LeftEnabled) _LeftAlert = digitalRead(CAMERAL_PIN_INT);
    if(_RightEnabled) _RightAlert = digitalRead(CAMERAR_PIN_INT);

    //Continue when no camera is reporting
    if(!_LeftAlert && !_RightAlert) return ErrorCodes::OK;

    //Check for new Data
    ErrorCodes err = Recieve();
    if(err != ErrorCodes::OK){
        _ui->ShowPopup("Victim:" + (err == ErrorCodes::invalid) ? "invalid" : "Already Found", (err == ErrorCodes::invalid) ? ErrorCodes::ERROR : ErrorCodes::info);
        return err;
    }

    //Dissect to side, and Victim Type
    char victim = _response[0];
    ErrorCodes side = (_response[1] == 'L') ? ErrorCodes::left : ErrorCodes::right;

    //Mapping call
    err = _mapper->SetVictim();
    if(err != ErrorCodes::OK) return err;

    _robot->endDrive(); //Stops robot

    //Get Amount of dropped Rescue Packs
    uint8_t amount;
    switch (victim) {
    case 'H':   //Harmed
        amount = 2;
        break;
    case 'S':   //Stable
        amount = 1;
        break;
    default:    //Unharmed / alles andere
        amount = 0;
        break;
    }

    _ui->ShowPopup("VICTIM Detected:" + victim, ErrorCodes::info);
    _ui->Update();
    _ejector->Eject(side, amount, _robot);
    _ui->LED_BUZZER_Signal(500,500,5);
    _ui->Update();

    //2RC - Harmed
    //1RC - Stable
    //0RC - Unharmed
    return ErrorCodes::OK;
}