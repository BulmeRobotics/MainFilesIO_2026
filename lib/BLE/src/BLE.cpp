#include "Bluetooth.h"
//@author Vincent Rohkamm
//@date 07.07.2025
//@brief 

#ifdef _MSC_VER
#pragma region PRIVATE FUNCTIONS
#endif

#ifdef _MSC_VER
#pragma endregion PRIVATE FUNCTIONS
#pragma region PUBLIC FUNCTIONS
#endif

ErrorCodes BT_commObj::init(void){
    if(!_ENABLE_BLE) return ErrorCodes::disabled;   //Check if BLE Enabled
    if(!BLE.begin()){
        if(_ENABLE_DEBUG) Serial.println("BLE init failed");
        return ErrorCodes::I2CError; //BLE init failed
    }
    return ErrorCodes::OK; //BLE init successful
}

ErrorCodes BT_commObj::connect(void){
    if(!_ENABLE_BLE) return ErrorCodes::disabled;   //Check if BLE Enabled
    uint32_t startTime = millis();
    BLE.scanForUuid(serviceUUID);
    if(p_UI != nullptr){
        p_UI->addBLEInfo("BLE", "Scan for UUID", true);
        p_UI->addBLEInfo("UUID:", serviceUUID, true);
    } 

    while(startTime + timeoutTime > millis()){
        //SCAN----------------------------------------------------------------------------------------------
        peripheral = BLE.available();   //Check if BLE peripheral is available

        if(peripheral){
            //Print adress, local Name
            if(p_UI != nullptr) {
                p_UI->addBLEInfo("BLE", "Device found", true);
                p_UI->addBLEInfo("LocalName:", peripheral.localName(), true);
                p_UI->addBLEInfo("Address:", peripheral.address(), true);
            }
            if(_ENABLE_DEBUG){  //DEBUG output Peripheral device:
                Serial.print("Found BLE device: ");
                Serial.print(peripheral.address());
                Serial.print(" '");
                Serial.print(peripheral.localName());
                Serial.print("' ");
                Serial.print(peripheral.advertisedServiceUuid());
                Serial.println();
            }
            
            //Check Name:
            if(peripheral.localName() != "ESP32-INFO") {
                if(p_UI != nullptr) p_UI->addBLEInfo("BLE", "Invalid Name", false);
                return ErrorCodes::invalid; //Only connect to ESP32-INFO device
            }

            BLE.stopScan();
            
        //CONNECT------------------------------------------------------------------------------------------------
            if(peripheral.connect()){
                if(p_UI != nullptr) p_UI->addBLEInfo("BLE", "Connected", true);
                if(_ENABLE_DEBUG) Serial.println("connected to BLE device");
            } else{
                if(p_UI != nullptr) p_UI->addBLEInfo("BLE", "Connection failed", false);
                return ErrorCodes::connectionError;
            }

        //DISCOVER ATTRIBUTES--------------------------------------------------------------------------------------
            if(peripheral.discoverAttributes()){
                if(p_UI != nullptr) p_UI->addBLEInfo("peripheral", "Attributes found", true);   
            } else {
                if(p_UI != nullptr) p_UI->addBLEInfo("BLE", "no Attributes found", false);
                peripheral.disconnect();
                if(p_UI != nullptr) p_UI->addBLEInfo("peripheral", "disconnected", false);
                return ErrorCodes::connectionError;
            }
        
        //CHECK CHARACTERISTICS------------------------------------------------------------------------------------
            rxCharacteristic = peripheral.characteristic(rxUUID);
            /*if(p_UI != nullptr) {
                char buffer[30] = "";
                sprintf(buffer,"num: %d", peripheral.characteristicCount());
                p_UI->addBLEInfo("Characteristics:", buffer, true);
            }*/
            if (!rxCharacteristic) {
                if(_ENABLE_DEBUG) Serial.println("Peripheral does not have ESP32-INFO characteristic!");
                if(p_UI != nullptr) p_UI->addBLEInfo("RX", "Invalid Characteristic", false);
                peripheral.disconnect();
                return ErrorCodes::invalid;
            } else if (!rxCharacteristic.canRead()) {
                if(_ENABLE_DEBUG) Serial.println("Peripheral does not have Read characteristic!");
                if(p_UI != nullptr) p_UI->addBLEInfo("RX", "cannot Read", false);
                peripheral.disconnect();
                return ErrorCodes::invalid;
            }
            if(p_UI != nullptr) p_UI->addBLEInfo("RX", "connected", true);

            txCharacteristic = peripheral.characteristic(txUUID);
            if (!txCharacteristic) {
                if(_ENABLE_DEBUG) Serial.println("Peripheral does not have ESP32-INFO characteristic!");
                if(p_UI != nullptr) p_UI->addBLEInfo("TX", "Invalid Characteristic", false);
                peripheral.disconnect();
                return ErrorCodes::invalid;
            } else if (!txCharacteristic.canWrite()) {
                if(_ENABLE_DEBUG) Serial.println("Peripheral does not have a writable ESP32-INFO characteristic!");
                if(p_UI != nullptr) p_UI->addBLEInfo("TX", "cannot Write", false);
                peripheral.disconnect();
                return ErrorCodes::invalid;
            }
            if(p_UI != nullptr) p_UI->addBLEInfo("TX", "connected", true);
            return ErrorCodes::OK; //Successfully connected to BLE device
        }
    }
    if(p_UI != nullptr) p_UI->addBLEInfo("BLE", "Timeout!", false);
    return ErrorCodes::notFound; //No BLE device found within timeout
}

ErrorCodes BT_commObj::writeData(uint8_t data){
    if(!_ENABLE_BLE) return ErrorCodes::disabled;   //Check if BLE Enabled
    if(peripheral.connected()){
        if(_ENABLE_DEBUG){
            Serial.print("Writing data to BLE device: ");
            Serial.println(data);
        }
        txCharacteristic.writeValue((byte)data);
        return ErrorCodes::OK;
    }
    else return ErrorCodes::connectionError;
}

uint8_t BT_commObj::readData(void){
    if(!_ENABLE_BLE) return 0;   //Check if BLE Enabled
    if(peripheral.connected()){
        uint8_t buffer = 0;
        rxCharacteristic.readValue(buffer);
        return buffer;
    }
    else return 0;
}

ErrorCodes BT_commObj::checkEN(void){
    if(_ENABLE_BLE) return ErrorCodes::OK;
    else return ErrorCodes::disabled;
}

#ifdef _MSC_VER
#pragma endregion PUBLIC FUNCTIONS
#endif