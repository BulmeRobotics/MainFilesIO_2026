#include "Vcameras.h"
#include <UserInterface.h>

// --- Definition der statischen Member-Variablen ---
mbed::UnbufferedSerial Vcameras::_camL(Vcameras::CAMERAL_TX, Vcameras::CAMERAL_RX);
mbed::UnbufferedSerial Vcameras::_camR(Vcameras::CAMERAR_TX, Vcameras::CAMERAR_RX);


char Vcameras::_buffL[7];
char Vcameras::_buffR[7];
uint8_t Vcameras::_idL = 0;
uint8_t Vcameras::_idR = 0;
bool Vcameras::_NEW_DATA_L = false;
bool Vcameras::_NEW_DATA_R = false;

//---------------------------------------------------------------------------------------------------------
// ISR
//---------------------------------------------------------------------------------------------------------

void Vcameras::on_camL_int(){
    char c;
    if(_camL.read(&c,1)){
        _buffL[_idL++] = c;
        if(c == '>') _NEW_DATA_L = true;
    }
}
void Vcameras::on_camR_int(){
    char c;
    if(_camR.read(&c,1)){
        _buffR[_idR++] = c;
        if(c == '>') _NEW_DATA_R = true;
    }
}

//---------------------------------------------------------------------------------------------------------
// Initialization
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Init(Ejector* ejector, Mapping* mapper, Driving* robot, UserInterface* ui){
    _ejector = ejector;
    _mapper = mapper;
    _robot = robot;
    _ui = ui;

    //Reset incoming:
    _NEW_DATA_L = false;
    _NEW_DATA_R = false;
    _idL = 0;
    _idR = 0;

    pinMode(CAMERAL_PIN_INT, INPUT);
    pinMode(CAMERAR_PIN_INT, INPUT);

    _camL.baud(115200);
    _camR.baud(115200);

    _camL.attach(&Vcameras::on_camL_int, mbed::SerialBase::RxIrq);
    _camR.attach(&Vcameras::on_camR_int, mbed::SerialBase::RxIrq);

    const char msg[] = "<I>";
    String str;

    _camL.write(msg, sizeof(msg) - 1);
    str = Recieve(ErrorCodes::left, CAM_TIMEOUT);
    _connectedL = (str.indexOf("OK") != -1) ? true : false;
    
    _camR.write(msg, sizeof(msg) - 1);
    str = Recieve(ErrorCodes::right, CAM_TIMEOUT);
    _connectedR = (str.indexOf("OK") != -1) ? true : false;

    if(!_connectedL || !_connectedR) return ErrorCodes::no_connection;
    return ErrorCodes::OK;  
}

//---------------------------------------------------------------------------------------------------------
// Recieve UART
//---------------------------------------------------------------------------------------------------------

String Vcameras::Recieve(ErrorCodes side, uint32_t waittime){
    bool* newData = (side == ErrorCodes::left) ? &_NEW_DATA_L : &_NEW_DATA_R;
    //Wait if wanted
    if(waittime > 0){
        uint32_t time = millis();
        while((time + waittime > millis()) && !(*newData)) delay(5);
    }

    if(*newData){
        *newData = false;
        char* buff = (side == ErrorCodes::left) ? _buffL : _buffR;
        uint8_t* len = (side == ErrorCodes::left) ? &_idL : &_idR;

        String str = "";
        for(uint8_t i = 0; i < *len; i++){
            str += buff[i];
            buff[i] = 0;
        }
        *len = 0;
        if(_debug_ifc != nullptr) _debug_ifc->println("Rec: " + str);

        int idx = str.indexOf('>',1);
        String s = (idx > 1) ? str.substring(1, idx) : " ";
        if(_debug_ifc != nullptr) _debug_ifc->println("Rec: " + s);
        return s;
    }
    return " ";
}

//---------------------------------------------------------------------------------------------------------
// Enable
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Enable(bool en, ErrorCodes side){
    bool conn = (side == ErrorCodes::left) ? _connectedL : _connectedR;
    if(!conn) return ErrorCodes::no_connection; //Return if no connection

    //Create buffers
    bool& enState =  (side == ErrorCodes::left) ? _LeftEnabled : _RightEnabled;
    mbed::UnbufferedSerial* ifc = (side == ErrorCodes::left) ? &_camL : &_camR;
    String str;

    //Send command
    const char* cmd = en ? "<E>" : "<D>";
    ifc->write(cmd, 3);

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
// Update
//---------------------------------------------------------------------------------------------------------

ErrorCodes Vcameras::Update(bool onRed, bool wallL, bool wallR){
    if(!_connectedL || !_connectedR) return ErrorCodes::no_connection;
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
    if(_NEW_DATA_L){
        str = Recieve(ErrorCodes::left);
        side = ErrorCodes::left;
    } else if (_NEW_DATA_R){
        str = Recieve(ErrorCodes::right);
        side = ErrorCodes::right;
    } else return ErrorCodes::OK;

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