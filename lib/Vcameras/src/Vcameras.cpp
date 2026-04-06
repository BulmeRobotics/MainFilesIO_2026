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

    //Reset incoming:
    

    pinMode(CAMERAL_PIN_INT, INPUT);
    pinMode(CAMERAR_PIN_INT, INPUT);

    _camL->begin(115200);
    _camR->begin(115200);

    if(_debug_ifc != nullptr) _debug_ifc->println("Start Cam INIT");

    String str;

    _camL->print("<I>");
    str = Recieve(ErrorCodes::left, CAM_TIMEOUT);
    _connectedL = (str.indexOf("OK") != -1) ? true : false;
    
    _camR->print("<I>");
    str = Recieve(ErrorCodes::right, CAM_TIMEOUT);
    _connectedR = (str.indexOf("OK") != -1) ? true : false;

    if(!_connectedL || !_connectedR) return ErrorCodes::no_connection;
    return ErrorCodes::OK;  
}

//---------------------------------------------------------------------------------------------------------
// Recieve UART
//---------------------------------------------------------------------------------------------------------

String Vcameras::Recieve(ErrorCodes side, uint32_t waittime){
    UART* _ifc = (side == ErrorCodes::left) ? _camL : _camR;
    //Wait if wanted
    String str = "";
    uint32_t startTime = millis();

    while (millis() - startTime <= waittime || waittime == 0) {
        while (_ifc->available()) {
            char c = _ifc->read();
            if (c == '<') {
                str = ""; // String bei neuem Paket zurücksetzen
            } else if (c == '>') {
                if(_debug_ifc != nullptr) _debug_ifc->println("Rec: " + str);
                return str; // Komplettes Paket empfangen!
            } else {
                str += c;
            }
        }
        if (waittime == 0) break; // Kein Blockieren, wenn waittime 0 ist
        delay(1);
    }
    return " "; // Timeout
}

//---------------------------------------------------------------------------------------------------------
// Enable
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Enable(bool en, ErrorCodes side){
    bool conn = (side == ErrorCodes::left) ? _connectedL : _connectedR;
    if(!conn) return ErrorCodes::no_connection; //Return if no connection

    //Create buffers
    bool& enState =  (side == ErrorCodes::left) ? _LeftEnabled : _RightEnabled;
    UART* ifc = (side == ErrorCodes::left) ? _camL : _camR;
    String str;

    //Send command
    const char* cmd = en ? "<E>" : "<D>";
    ifc->print(cmd);

    //Recieve Command
    str = Recieve(side, CAM_TIMEOUT);
    if(str.indexOf("OK") != -1) {
        enState = en;
        return ErrorCodes::OK;
    }        
    _ui->ShowPopup("cams enable error", ErrorCodes::ERROR);
    return ErrorCodes::invalid;
}

//---------------------------------------------------------------------------------------------------------
// Handle Reset
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::HandleReset(){
    if(!_victimFound) return ErrorCodes::OK;

    if(_timeFound + DEACT_TIME_VICTIM < millis()){
        _victimFound = false;
        //Enable cams
        Enable(true,ErrorCodes::left);
        Enable(true,ErrorCodes::right);
        return ErrorCodes::OK;
    }
    return ErrorCodes::disabled;
}

//---------------------------------------------------------------------------------------------------------
// Update
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Update(bool onRed, bool wallL, bool wallR){
    if(!_connectedL || !_connectedR) return ErrorCodes::no_connection;
    if(HandleReset() == ErrorCodes::disabled) return ErrorCodes::disabled;
    if(_oldRed && !onRed) {
        Enable(true, ErrorCodes::left);
        Enable(true, ErrorCodes::right);
    } else if (!_oldRed && onRed){
        Enable(false, ErrorCodes::left);
        Enable(false, ErrorCodes::right);
    }
    _oldRed = onRed;
    if(onRed) return ErrorCodes::OK;

    //Wände überprüfen
    if(wallL && !_LeftEnabled)      Enable(true,  ErrorCodes::left);
    else if(!wallL && _LeftEnabled) Enable(false, ErrorCodes::left);

    if(wallR && !_RightEnabled)      Enable(true,  ErrorCodes::right);
    else if(!wallR && _RightEnabled) Enable(false, ErrorCodes::right);

    //Abfrage auf alert
    if(_LeftEnabled) _LeftAlert = digitalRead(CAMERAL_PIN_INT);
    if(_RightEnabled) _RightAlert = digitalRead(CAMERAR_PIN_INT);

    //Continue when no camera is reporting
    if(!_LeftAlert && !_RightAlert) return ErrorCodes::OK;

    String str = "";
    ErrorCodes side;
    //Wait for new Data
    if(_LeftAlert){
        str = Recieve(ErrorCodes::left);
        if(str[0] != ' ')
            side = ErrorCodes::left;
        else return ErrorCodes::OK;
    } else if(_RightAlert){
        str = Recieve(ErrorCodes::right);
        if(str[0] != ' ')
            side = ErrorCodes::right;
        else return ErrorCodes::OK;
    }
    
    //Dissect to side, and Victim Type
    char victim = str[0];

    //Check if Victim is allowed:
    if(!(victim == 'H' || victim == 'S' || victim == 'U')){
        _ui->ShowPopup("Invalid victim!", ErrorCodes::warning, 2);
        return ErrorCodes::ERROR;
    }

    //Mapping call
    ErrorCodes err = _mapper->SetVictim();
    if(err != ErrorCodes::OK) return err;

    _robot->endDrive(); //Stops robot

    //Reset cams
    _victimFound = true;
    Enable(false, ErrorCodes::left);
    Enable(false, ErrorCodes::right);

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
    char buffer[20];
    sprintf(buffer,"VICTIM Detected: %c", victim);
    _ui->ShowPopup(buffer, ErrorCodes::info);
    _ui->Update();
    _ejector->Eject(side, amount, _robot);
    _ui->LED_BUZZER_Signal(500,500,5);
    _ui->Update();

    //2RC - Harmed
    //1RC - Stable
    //0RC - Unharmed
    return ErrorCodes::OK;
}