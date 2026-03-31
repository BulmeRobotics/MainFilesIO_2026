#include "ColorSensing.h"


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

ErrorCodes EEPROM::ReadColorSens(PoI_Type type, char sensor, uint32_t* buffer){
    uint16_t curAddr = GetStartAddr(type, sensor);
    for(uint8_t i=0; i<EEPROM_PACKAGE_NUM; i++, curAddr += (EEPROM_PACKAGE_SIZE + EEPROM_PACKAGE_OVERHEAD)){
        uint8_t localBuffer[EEPROM_PACKAGE_SIZE];
		i2ceeprom.read(curAddr, localBuffer, EEPROM_PACKAGE_SIZE);
		memcpy((void*)&buffer[i+1], localBuffer, EEPROM_PACKAGE_SIZE);
    }
    return ErrorCodes::OK;
}

ErrorCodes EEPROM::WriteColorSens(PoI_Type type, char sensor, uint32_t* buffer){
    int curAddr = GetStartAddr(type, sensor);
	for (int i = 0; i < EEPROM_PACKAGE_NUM; i++, curAddr += (EEPROM_PACKAGE_SIZE + EEPROM_PACKAGE_OVERHEAD)) {
		uint8_t localBuffer[EEPROM_PACKAGE_SIZE];
		memcpy(localBuffer, (void*)&buffer[i+1], EEPROM_PACKAGE_SIZE);
		i2ceeprom.write(curAddr, localBuffer, EEPROM_PACKAGE_SIZE);
	}
	return ErrorCodes::OK;
}

// COLOR SENSING: --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t ColorSensing::Init(TwoWire* wire){
    uint8_t ebuff = 0;
    TCA9548A(0, MULTIPLEX_ADRESS);
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
    TCA9548A(1, MULTIPLEX_ADRESS);
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
    ReadAllColorSensFromEEPROM();
    return ebuff;
}



PoI_Type ColorSensing::GetFloor(){
    return floorComb;
}