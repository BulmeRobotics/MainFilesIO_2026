/**
* @name:    TofSensors.cpp
* @date:	18.11.2025
* @authors: Florian Wiesner
* @details: .cpp file for Time-of-Flight-Sensors 
*/

// Libraries
#include "TofSensors.h"

#ifdef _MSC_VER
    #pragma region Parent Class //-----------------------------------------------------------------------------------------------
#endif
TofParent::TofParent(uint8_t i2cAddr, uint8_t xshut)
{
    xshutPin = xshut;
    i2cAddress = i2cAddr;
    lastMeasurement = 0;
    measurementCount = 0;
    ts_lastMeasurement = 0;
    newData = false;
    lastStatus = static_cast<TofStatus>(0);
}

void TofParent::Off(void) {
    pinMode(xshutPin, OUTPUT);
    digitalWrite(xshutPin, LOW);
}

void TofParent::On(void) {
    pinMode(xshutPin, INPUT_PULLUP);
}

#ifdef _MSC_VER
    #pragma endregion
    #pragma region VL6180X //----------------------------------------------------------------------------------------------------
#endif
TofVL180X::TofVL180X(uint8_t i2cAddress , uint8_t xshutPin)
    : TofParent(i2cAddress, xshutPin) {}

bool TofVL180X::Init(void) {
    On();
	delay(100);
	sensor.setTimeout(500);
	sensor.init();
	sensor.configureDefault();
	sensor.writeReg(VL6180X::SYSRANGE__MAX_CONVERGENCE_TIME, 15);
	sensor.writeReg16Bit(VL6180X::SYSALS__INTEGRATION_PERIOD, 20);
	sensor.setTimeout(500);
	delay(100);
	sensor.stopContinuous();
	sensor.setTimeout(500);
	delay(100);
	sensor.setAddress(i2cAddress);
	sensor.startRangeContinuous(RANGING_BUDGET_SHORT);
    return true;
}

ErrorCodes TofVL180X::Read(void) {
    uint8_t update = sensor.readRangeContinuous();
    if (update != 0) {
        lastMeasurement = update;
        ts_lastMeasurement = millis();
        newData = true;
        measurementCount++;

        if (lastMeasurement < 255) {
            lastStatus = TofStatus::VALID;
            return ErrorCodes::NEW_DATA;
        }
        else {
            lastStatus = TofStatus::OUT_OF_RANGE;
            return ErrorCodes::OUT_OF_RANGE;
        }
    }
    if (sensor.timeoutOccurred()) return ErrorCodes::TIMEOUT;
    else return ErrorCodes::OK;

    

    
}

uint16_t TofVL180X::GetRange(void) {
    return lastMeasurement;
}

TofStatus TofVL180X::GetStatus(void) {
    return lastStatus;
}

ErrorCodes TofVL180X::Stop(void) {
    sensor.stopContinuous();
    delay(5);
    newData = false;
    ts_lastMeasurement = millis();

    if (sensor.getTimeout()) return ErrorCodes::TIMEOUT;
    else return ErrorCodes::OK;
}

ErrorCodes TofVL180X::Continue(void) {
    sensor.startRangeContinuous(RANGING_BUDGET_SHORT);
    return ErrorCodes::OK;
}

#ifdef _MSC_VER
    #pragma endregion
    #pragma region VL53L4CD //---------------------------------------------------------------------------------------------------
#endif
TofVL53L4CD::TofVL53L4CD(uint8_t i2cAddress , uint8_t xshutPin)
    : TofParent(i2cAddress, xshutPin), sensor(&Wire, xshutPin) {}

bool TofVL53L4CD::Init(void) {
    On();
	delay(10);
	sensor.begin();
	sensor.VL53L4CD_Off();
	sensor.InitSensor(i2cAddress);
	sensor.VL53L4CD_SetRangeTiming(RANGING_BUDGET_MID, 0);
	sensor.VL53L4CD_StartRanging();
    return true;
}

ErrorCodes TofVL53L4CD::Read(void) {
	uint8_t NewDataReady = 0;
	VL53L4CD_Result_t results;
	uint8_t status;

	do {
		status = sensor.VL53L4CD_CheckForDataReady(&NewDataReady);
	} while (!NewDataReady);

	if ((!status) && (NewDataReady != 0)) {
		//(Mandatory) Clear HW interrupt to restart measurements
		sensor.VL53L4CD_ClearInterrupt();

		//Read measured distance. RangeStatus = 0 means valid data
		sensor.VL53L4CD_GetResult(&results);
	}
	
    // !!!----!!!!----calculateTrueRange_constant(&results.distance_mm, rangeCoeff_midFront_constant);

    ts_lastMeasurement = millis();
    newData = true;
    measurementCount++;

	// Falls der Messwert ungültig ist (außer Reichweite)
	if (!results.range_status) {
        lastMeasurement = results.distance_mm;
        lastStatus = TofStatus::VALID;
        return ErrorCodes::NEW_DATA;
    }
	else {
        lastMeasurement = 1023;
        lastStatus = TofStatus::OUT_OF_RANGE;
        return ErrorCodes::OUT_OF_RANGE;
    }
}

uint16_t TofVL53L4CD::GetRange(void) {
    return lastMeasurement;
}

TofStatus TofVL53L4CD::GetStatus(void) {
    return lastStatus;
}

ErrorCodes TofVL53L4CD::Stop(void) {
    sensor.VL53L4CD_StopRanging();
    return ErrorCodes::OK;
}

ErrorCodes TofVL53L4CD::Continue(void) {
    sensor.VL53L4CD_StartRanging();
    return ErrorCodes::OK;
}

#ifdef _MSC_VER
    #pragma endregion
    #pragma region TOF Class //--------------------------------------------------------------------------------------------------
#endif
ErrorCodes TofSensors::Init(void) {
    if (shortLeftBack.Init() == true && shortLeftFront.Init() == true && shortRightFront.Init() == true 
        && shortRightBack.Init() == true && midBack.Init() == true && midFront.Init() == true)
        return ErrorCodes::OK;
    else 
        return ErrorCodes::ERROR;
}

ErrorCodes TofSensors::Update(void) {
    if (shortLeftBack.Read() == ErrorCodes::NEW_DATA || shortLeftFront.Read() == ErrorCodes::NEW_DATA
        || shortRightFront.Read() == ErrorCodes::NEW_DATA || shortRightBack.Read() == ErrorCodes::NEW_DATA
        || midBack.Read() == ErrorCodes::NEW_DATA || midFront.Read() == ErrorCodes::NEW_DATA)
        return ErrorCodes::NEW_DATA;
    else 
        return ErrorCodes::OK;
}

uint16_t TofSensors::GetRange(TofType sensor) {
    switch (sensor)
    {
    case TofType::SHORT_LEFT_BACK:
        return shortLeftBack.GetRange();
        break;
    
    case TofType::SHORT_LEFT_FRONT:
        return shortLeftFront.GetRange();
        break;
    
    case TofType::SHORT_RIGHT_FRONT:
        return shortRightFront.GetRange();
        break;

    case TofType::SHORT_RIGHT_BACK:
        return shortRightBack.GetRange();
        break;

    case TofType::MID_BACK:
        return midBack.GetRange();
        break;
    
    case TofType::MID_FRONT:
        return midFront.GetRange();
        break;
    
    default:
        return 0;
        break;
    }
}

TofStatus TofSensors::GetStatus(TofType sensor) {
    switch (sensor)
    {
    case TofType::SHORT_LEFT_BACK:
        return shortLeftBack.GetStatus();
        break;
    
    case TofType::SHORT_LEFT_FRONT:
        return shortLeftFront.GetStatus();
        break;
    
    case TofType::SHORT_RIGHT_FRONT:
        return shortRightFront.GetStatus();
        break;

    case TofType::SHORT_RIGHT_BACK:
        return shortRightBack.GetStatus();
        break;

    case TofType::MID_BACK:
        return midBack.GetStatus();
        break;
    
    case TofType::MID_FRONT:
        return midFront.GetStatus();
        break;
    
    default:
        return TofStatus::TIMEOUT;
        break;
    }
}
#ifdef _MSC_VER
    #pragma endregion
#endif
