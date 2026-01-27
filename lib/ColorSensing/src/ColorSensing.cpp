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