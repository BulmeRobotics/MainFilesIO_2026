#include "ColorSensing.h"
#include <CustomDatatypes.h>


int16_t EEPROM::GetStartAddr(PoI_Type type, char sensor){
    if (sensor == 'R')
        return EEPROM_START_ADD_REFLECTIVE;
    
    switch (type) {
    case PoI_Type::white:
        return (sensor == 'F') ? EEPROM_START_ADD_FRONT_WHITE : EEPROM_START_ADD_MIDDLE_WHITE;
	    break;
    case(PoI_Type::blue):
        return (sensor == 'F') ? EEPROM_START_ADD_FRONT_BLUE : EEPROM_START_ADD_MIDDLE_BLUE;
        break;
    case(PoI_Type::checkpoint):
        return (sensor == 'F') ? EEPROM_START_ADD_FRONT_CP : EEPROM_START_ADD_MIDDLE_CP;
        break;
    case(PoI_Type::black):
        return (sensor == 'F') ? EEPROM_START_ADD_FRONT_BLACK : EEPROM_START_ADD_MIDDLE_BlACK;
        break;
    case(PoI_Type::dangerZone):
        return (sensor == 'F') ? EEPROM_START_ADD_FRONT_DZ : EEPROM_START_ADD_MIDDLE_DZ;
        break;
    default:
        break;
    }
}

ErrorCodes EEPROM::ReadFromEEPROM(PoI_Type type, char sensor, uint16_t* buffer){
    int16_t curAddr = GetStartAddr(type, sensor);

    for(uint8_t i=0; i<EEPROM_PACKAGE_NUM; i++, curAddr += (EEPROM_PACKAGE_SIZE + EEPROM_PACKAGE_OVERHEAD)){
        uint8_t localBuffer[EEPROM_PACKAGE_SIZE];
		i2ceeprom.read(curAddr, localBuffer, EEPROM_PACKAGE_SIZE);
		memcpy((void*)&buffer[i], localBuffer, EEPROM_PACKAGE_SIZE);
    }
    return ErrorCodes::OK;
}

ErrorCodes EEPROM::WriteToEEPROM(PoI_Type type, char sensor, uint16_t* buffer){
    int16_t curAddr = GetStartAddr(type, sensor);
	for (int i = 0; i < EEPROM_PACKAGE_NUM; i++, curAddr += (EEPROM_PACKAGE_SIZE + EEPROM_PACKAGE_OVERHEAD)) {
		uint8_t localBuffer[EEPROM_PACKAGE_SIZE];
		memcpy(localBuffer, (void*)&buffer[i], EEPROM_PACKAGE_SIZE);
		i2ceeprom.write(curAddr, localBuffer, EEPROM_PACKAGE_SIZE);
	}
	return ErrorCodes::OK;
}

// COLOR SENSING: --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ColorSensing::Init(TwoWire* wire, UserInterface* ui, EEPROM* eeprom){
    _eeprom = eeprom;
    _ui = ui;
    uint8_t ebuff = 0;
    TCA9548A(0);

    if (!front.begin(0x39, wire, 0)) ebuff |= 1 << 0;
	//LED
    if (!(ebuff & 0x01)) {
        if (!front.setLEDCurrent(LED_CURRENT))	    ebuff |= 1 << 2;
        if (!front.enableLED(_ENABLE_LED_FRONT))	ebuff |= 1 << 2;
    }
    if (!front.setGain(AS7341_GAIN_256X))   ebuff |= 1 << 4;
    if (!front.setATIME(ATIME_Front))       ebuff |= 1 << 7;
    if (!front.setASTEP(ASTEP_Front))       ebuff |= 1 << 7;

        //Middle Color Sensor
    TCA9548A(1);
    if (!middle.begin(0x39, wire, 0))	ebuff |= 1 << 1;

    //Set LED Current and turn on
    if (!(ebuff & 1 << 1)) {
        if (!middle.setLEDCurrent(LED_CURRENT))	    ebuff |= 1 << 3;
        if (!middle.enableLED(_ENABLE_LED_MIDDLE))	ebuff |= 1 << 3;
    }
    if (!middle.setGain(AS7341_GAIN_256X))  ebuff |= 1 << 5;
    if (!middle.setATIME(ATIME_Middle))     ebuff |= 1 << 6;
    if (!middle.setASTEP(ASTEP_Middle))     ebuff |= 1 << 6;

    //EEPROM
    _eeprom->ReadFromEEPROM(PoI_Type::white,      'F', (uint16_t*)&frontColorsCalibrated[WHITE]);
    _eeprom->ReadFromEEPROM(PoI_Type::black,      'F', (uint16_t*)&frontColorsCalibrated[BLACK]);
    _eeprom->ReadFromEEPROM(PoI_Type::blue,       'F', (uint16_t*)&frontColorsCalibrated[BLUE]);
    _eeprom->ReadFromEEPROM(PoI_Type::red,        'F', (uint16_t*)&frontColorsCalibrated[RED]);
    _eeprom->ReadFromEEPROM(PoI_Type::checkpoint, 'F', (uint16_t*)&frontColorsCalibrated[SILVER]);
    
    _eeprom->ReadFromEEPROM(PoI_Type::white,      'M', (uint16_t*)&middleColorsCalibrated[WHITE]);
    _eeprom->ReadFromEEPROM(PoI_Type::black,      'M', (uint16_t*)&middleColorsCalibrated[BLACK]);
    _eeprom->ReadFromEEPROM(PoI_Type::blue,       'M', (uint16_t*)&middleColorsCalibrated[BLUE]);
    _eeprom->ReadFromEEPROM(PoI_Type::red,        'M', (uint16_t*)&middleColorsCalibrated[RED]);
    _eeprom->ReadFromEEPROM(PoI_Type::checkpoint, 'M', (uint16_t*)&middleColorsCalibrated[SILVER]);
  
    return ebuff;
}


ErrorCodes ColorSensing::EnableRead(bool enable){
    //Turn On
    if(!_READING && enable){
        TCA9548A(0);
        if(!front.startReading()) return ErrorCodes::ERROR;
        TCA9548A(1);
        if(!middle.startReading()) return ErrorCodes::ERROR;
        _READING = true;
    }

    //Turn Off
    if(_READING && !enable){
        uint32_t time = millis();
        TCA9548A(0);
        while(!front.checkReadingProgress()){
            if(time + COLOR_TIMEOUT < millis()) return ErrorCodes::TIMEOUT;
            delay(5);
        }
        TCA9548A(1);
        while(!middle.checkReadingProgress()){
            if(time + COLOR_TIMEOUT < millis()) return ErrorCodes::TIMEOUT;
            delay(5);
        }
        _READING = false;
    }
}


ErrorCodes ColorSensing::Update(){
    if(_READING){
        TCA9548A(0);
        if(front.checkReadingProgress()){
            if(!_FREEZE_SENSOR) colorFront = checkFront();
            if(!front.startReading()) return ErrorCodes::ERROR;
        }
        TCA9548A(1);
        if(middle.checkReadingProgress()){
            if(!_FREEZE_SENSOR) colorMiddle = checkMiddle();
            if(!middle.startReading()) return ErrorCodes::ERROR;
        }
        return ErrorCodes::OK;
    }
    return ErrorCodes::disabled;
}

PoI_Type ColorSensing::GetFloor(){
    return floorComb;
}

void ColorSensing::Freeze(bool enable){
    _FREEZE_SENSOR = enable;
}

bool ColorSensing::GetAlert(){
    return _ALERT;
}

void ColorSensing::printDebugData(uint16_t* rawColor){
    if(_debugPort != nullptr){
        for (uint8_t i = 0; i < 12; i++)
        {
            _debugPort->print(rawColor[i]);
            _debugPort->print("\t");
        }
        _debugPort->println();    
    }
}

PoI_Type ColorSensing::checkFront(){
    //Read all channels
    TCA9548A(0);
    uint16_t colorRaw[10];
    colorRaw[0] = front.getChannel(AS7341_CHANNEL_415nm_F1);
    colorRaw[1] = front.getChannel(AS7341_CHANNEL_445nm_F2);
    colorRaw[2] = front.getChannel(AS7341_CHANNEL_480nm_F3);
    colorRaw[3] = front.getChannel(AS7341_CHANNEL_515nm_F4);
    colorRaw[4] = front.getChannel(AS7341_CHANNEL_555nm_F5);
    colorRaw[5] = front.getChannel(AS7341_CHANNEL_590nm_F6);
    colorRaw[6] = front.getChannel(AS7341_CHANNEL_630nm_F7);
    colorRaw[7] = front.getChannel(AS7341_CHANNEL_680nm_F8);
    colorRaw[8] = front.getChannel(AS7341_CHANNEL_CLEAR);
    colorRaw[9] = front.getChannel(AS7341_CHANNEL_NIR);

    //Only prints values when debugPort is set, otherwise does nothing
    printDebugData(colorRaw);

    

    return PoI_Type::white; //TODO: Implement actual color checking
}

PoI_Type ColorSensing::checkMiddle(){
    //Read all channels
    TCA9548A(1);
    uint16_t colorRaw[10];
    colorRaw[0] = middle.getChannel(AS7341_CHANNEL_415nm_F1);
    colorRaw[1] = middle.getChannel(AS7341_CHANNEL_445nm_F2);
    colorRaw[2] = middle.getChannel(AS7341_CHANNEL_480nm_F3);
    colorRaw[3] = middle.getChannel(AS7341_CHANNEL_515nm_F4);
    colorRaw[4] = middle.getChannel(AS7341_CHANNEL_555nm_F5);
    colorRaw[5] = middle.getChannel(AS7341_CHANNEL_590nm_F6);
    colorRaw[6] = middle.getChannel(AS7341_CHANNEL_630nm_F7);
    colorRaw[7] = middle.getChannel(AS7341_CHANNEL_680nm_F8);
    colorRaw[8] = middle.getChannel(AS7341_CHANNEL_CLEAR);
    colorRaw[9] = middle.getChannel(AS7341_CHANNEL_NIR);

    //Only prints values when debugPort is set, otherwise does nothing
    printDebugData(colorRaw);

    return PoI_Type::white; //TODO: Implement actual color checking
}

ErrorCodes ColorSensing::Calibrate(PoI_Type type){
    uint32_t time = millis();

    _ui->ShowCalibrationScreen(type);

    //Finish old reading
    TCA9548A(0);
    while(!front.checkReadingProgress()){
        if(time + COLOR_TIMEOUT < millis()) {
            _ui->FinishCalibration(false);
            return ErrorCodes::TIMEOUT;
        }
        delay(5);
    }
    TCA9548A(1);
    while(!middle.checkReadingProgress()){
        if(time + COLOR_TIMEOUT < millis()) {
            _ui->FinishCalibration(false);
            return ErrorCodes::TIMEOUT;
        }
        delay(5);
    }


    //Update calibration screen:
    _ui->UpdateCalibrationProgress(1, RUNS_calibration + 1);

    uint32_t bufferFront[10] = {0};
    uint32_t bufferMiddle[10] = {0};

    bool frontReady;
    bool middleReady;

    //Read new values
    for (uint8_t i = 0; i<RUNS_calibration; i++){
        frontReady = false;
        middleReady = false;

        TCA9548A(1);
        middle.startReading();

        TCA9548A(0);
        front.startReading();

        while(!frontReady || !middleReady){
            delay(5);
            TCA9548A(0);
            if(!frontReady && front.checkReadingProgress()){
                bufferFront[0] += front.getChannel(AS7341_CHANNEL_415nm_F1);
                bufferFront[1] += front.getChannel(AS7341_CHANNEL_445nm_F2);
                bufferFront[2] += front.getChannel(AS7341_CHANNEL_480nm_F3);
                bufferFront[3] += front.getChannel(AS7341_CHANNEL_515nm_F4);
                bufferFront[4] += front.getChannel(AS7341_CHANNEL_555nm_F5);
                bufferFront[5] += front.getChannel(AS7341_CHANNEL_590nm_F6);
                bufferFront[6] += front.getChannel(AS7341_CHANNEL_630nm_F7);
                bufferFront[7] += front.getChannel(AS7341_CHANNEL_680nm_F8);
                bufferFront[8] += front.getChannel(AS7341_CHANNEL_CLEAR);
                bufferFront[9] += front.getChannel(AS7341_CHANNEL_NIR);
                frontReady = true;
            }
            TCA9548A(1);
            if(!middleReady && middle.checkReadingProgress()){
                bufferMiddle[0] += middle.getChannel(AS7341_CHANNEL_415nm_F1);
                bufferMiddle[1] += middle.getChannel(AS7341_CHANNEL_445nm_F2);
                bufferMiddle[2] += middle.getChannel(AS7341_CHANNEL_480nm_F3);
                bufferMiddle[3] += middle.getChannel(AS7341_CHANNEL_515nm_F4);
                bufferMiddle[4] += middle.getChannel(AS7341_CHANNEL_555nm_F5);
                bufferMiddle[5] += middle.getChannel(AS7341_CHANNEL_590nm_F6);
                bufferMiddle[6] += middle.getChannel(AS7341_CHANNEL_630nm_F7);
                bufferMiddle[7] += middle.getChannel(AS7341_CHANNEL_680nm_F8);
                bufferMiddle[8] += middle.getChannel(AS7341_CHANNEL_CLEAR);
                bufferMiddle[9] += middle.getChannel(AS7341_CHANNEL_NIR);
                middleReady = true;
            }
            if(time + COLOR_TIMEOUT < millis()) {
                _ui->FinishCalibration(false);
                return ErrorCodes::TIMEOUT;
            }

            _ui->UpdateCalibrationProgress(2, RUNS_calibration + 1);
        }

        uint8_t floor = 0;
        switch (type)
        {
        case PoI_Type::black:
            floor = BLACK;
            break;
        case PoI_Type::blue:
            floor = BLUE;
            break;
        case PoI_Type::checkpoint:
            floor = SILVER;
            break;
        case PoI_Type::red:
            floor = RED;
            break;
        
        default:
            floor = WHITE;
            break;
        }

        frontColorsCalibrated[floor].F1     = bufferFront[0]/RUNS_calibration;
        frontColorsCalibrated[floor].F2     = bufferFront[1]/RUNS_calibration;
        frontColorsCalibrated[floor].F3     = bufferFront[2]/RUNS_calibration;
        frontColorsCalibrated[floor].F4     = bufferFront[3]/RUNS_calibration;
        frontColorsCalibrated[floor].F5     = bufferFront[4]/RUNS_calibration;
        frontColorsCalibrated[floor].F6     = bufferFront[5]/RUNS_calibration;
        frontColorsCalibrated[floor].F7     = bufferFront[6]/RUNS_calibration;
        frontColorsCalibrated[floor].F8     = bufferFront[7]/RUNS_calibration;
        frontColorsCalibrated[floor].Clear  = bufferFront[8]/RUNS_calibration;
        frontColorsCalibrated[floor].NIR    = bufferFront[9]/RUNS_calibration;
        
        middleColorsCalibrated[floor].F1     = bufferMiddle[0]/RUNS_calibration;
        middleColorsCalibrated[floor].F2     = bufferMiddle[1]/RUNS_calibration;
        middleColorsCalibrated[floor].F3     = bufferMiddle[2]/RUNS_calibration;
        middleColorsCalibrated[floor].F4     = bufferMiddle[3]/RUNS_calibration;
        middleColorsCalibrated[floor].F5     = bufferMiddle[4]/RUNS_calibration;
        middleColorsCalibrated[floor].F6     = bufferMiddle[5]/RUNS_calibration;
        middleColorsCalibrated[floor].F7     = bufferMiddle[6]/RUNS_calibration;
        middleColorsCalibrated[floor].F8     = bufferMiddle[7]/RUNS_calibration;
        middleColorsCalibrated[floor].Clear  = bufferMiddle[8]/RUNS_calibration;
        middleColorsCalibrated[floor].NIR    = bufferMiddle[9]/RUNS_calibration;

        _eeprom->WriteToEEPROM(type, 'F', (uint16_t*)&frontColorsCalibrated[floor]);
        _eeprom->WriteToEEPROM(type, 'M', (uint16_t*)&middleColorsCalibrated[floor]);

        _ui->FinishCalibration(true);
        return ErrorCodes::OK;
    }
}
