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

    ts_waitStart = 0;
    ts_lastReadCall = 0;
    waitingForMeasurement = false;
    timeoutOccured = false;
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

bool TofParent::Init(void) { return false; }
ErrorCodes TofParent::Read(void) { return ErrorCodes::ERROR; }
uint16_t TofParent::GetRange(void) { return 0; }
TofStatus TofParent::GetStatus(void) { return TofStatus::ERROR; }
bool TofParent::IsDataNew(void) { return false; }
bool TofParent::TimeoutOccured(void) { return false; }
ErrorCodes TofParent::Stop(void) { return ErrorCodes::ERROR; }
ErrorCodes TofParent::Continue(void) { return ErrorCodes::ERROR; }

#ifdef _MSC_VER
    #pragma endregion
    #pragma region VL6180X //----------------------------------------------------------------------------------------------------
#endif
TofVL6180X::TofVL6180X(uint8_t i2cAddress , uint8_t xshutPin)
    : TofParent(i2cAddress, xshutPin) {}

bool TofVL6180X::Init(void) {
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

ErrorCodes TofVL6180X::Read(void) {
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

uint16_t TofVL6180X::GetRange(void) {
    return lastMeasurement;
}

TofStatus TofVL6180X::GetStatus(void) {
    return lastStatus;
}

bool TofVL6180X::IsDataNew(void) {
    return newData;
}

bool TofVL6180X::TimeoutOccured(void) {
    return timeoutOccured;
}

ErrorCodes TofVL6180X::Stop(void) {
    sensor.stopContinuous();
    delay(5);
    newData = false;
    waitingForMeasurement = false;

    if (sensor.getTimeout()) {
        timeoutOccured = true;
        lastStatus = TofStatus::TIMEOUT;
        return ErrorCodes::TIMEOUT;
    }

    timeoutOccured = false;
    return ErrorCodes::OK;
}

ErrorCodes TofVL6180X::Continue(void) {
    sensor.startRangeContinuous(RANGING_BUDGET_SHORT);
    timeoutOccured = false;
    waitingForMeasurement = false;
    ts_lastReadCall = millis();
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
    timeoutOccured = false;
    waitingForMeasurement = false;
    ts_lastReadCall = millis();
    return true;
}

// ErrorCodes TofVL53L4CD::Read(void) {
// 	uint8_t NewDataReady = 0;
// 	VL53L4CD_Result_t results;
// 	uint8_t status;

// 	do {
// 		status = sensor.VL53L4CD_CheckForDataReady(&NewDataReady);
// 	} while (!NewDataReady);

// 	if ((!status) && (NewDataReady != 0)) {
// 		//(Mandatory) Clear HW interrupt to restart measurements
// 		sensor.VL53L4CD_ClearInterrupt();

// 		//Read measured distance. RangeStatus = 0 means valid data
// 		sensor.VL53L4CD_GetResult(&results);
// 	}
	
//     // !!!----!!!!----calculateTrueRange_constant(&results.distance_mm, rangeCoeff_midFront_constant);

//     ts_lastMeasurement = millis();
//     newData = true;
//     measurementCount++;

// 	// Falls der Messwert ungültig ist (außer Reichweite)
// 	if (!results.range_status) {
//         lastMeasurement = results.distance_mm;
//         lastStatus = TofStatus::VALID;
//         return ErrorCodes::NEW_DATA;
//     }
// 	else {
//         lastMeasurement = 1023;
//         lastStatus = TofStatus::OUT_OF_RANGE;
//         return ErrorCodes::OUT_OF_RANGE;
//     }
// }

ErrorCodes TofVL53L4CD::Read(void)
{
    uint8_t newDataReady = 0;
    uint8_t rangeStatus = 255;
    uint16_t distance = 0;
    uint8_t status = 0;

    const uint32_t now = millis();

    // Wenn Read() lange nicht aufgerufen wurde, alten Wartezustand verwerfen.
    // Sonst würdest du nach 10 s Pause beim ersten neuen Aufruf sofort TIMEOUT bekommen.
    if ((now - ts_lastReadCall) > TIMEOUT_TIME_MS) {
        waitingForMeasurement = false;
    }
    ts_lastReadCall = now;

    // Nur wenn wir aktiv pollen, beginnt ein neuer Wartevorgang
    if (!waitingForMeasurement) {
        ts_waitStart = now;
        waitingForMeasurement = true;
    }

    status = sensor.VL53L4CD_CheckForDataReady(&newDataReady);
    if (status != 0) {
        waitingForMeasurement = false;
        timeoutOccured = false;
        lastStatus = TofStatus::ERROR;
        return ErrorCodes::ERROR;
    }

    if (newDataReady == 0) {
        if ((now - ts_waitStart) > TIMEOUT_TIME_MS) {
            waitingForMeasurement = false;
            timeoutOccured = true;
            lastMeasurement = INVALID_MEASUREMENT;
            lastStatus = TofStatus::TIMEOUT;
            return ErrorCodes::TIMEOUT;
        }

        return ErrorCodes::NO_NEW_DATA;
    }

    status = sensor.VL53L4CD_ClearInterrupt();
    if (status != 0) {
        waitingForMeasurement = false;
        timeoutOccured = false;
        lastStatus = TofStatus::ERROR;
        return ErrorCodes::ERROR;
    }

    status = sensor.VL53L4CD_GetMinimalResult(&rangeStatus, &distance);
    if (status != 0) {
        waitingForMeasurement = false;
        timeoutOccured = false;
        lastStatus = TofStatus::ERROR;
        return ErrorCodes::ERROR;
    }

    waitingForMeasurement = false;
    timeoutOccured = false;

    ts_lastMeasurement = now;
    newData = true;
    measurementCount++;

    // Optional:
    // calculateTrueRange_constant(&results.distance_mm, rangeCoeff_midFront_constant);

    if (rangeStatus == 0) {
        lastMeasurement = distance;
        lastStatus = TofStatus::VALID;
        return ErrorCodes::NEW_DATA;
    } else {
        lastMeasurement = INVALID_MEASUREMENT;
        lastStatus = TofStatus::OUT_OF_RANGE;
        return ErrorCodes::NEW_DATA;
    }
}

uint16_t TofVL53L4CD::GetRange(void) {
    newData = false;
    return lastMeasurement;
}

TofStatus TofVL53L4CD::GetStatus(void) {
    return lastStatus;
}

bool TofVL53L4CD::IsDataNew(void) {
    return newData;
}

bool TofVL53L4CD::TimeoutOccured(void) {
    return timeoutOccured;
}

ErrorCodes TofVL53L4CD::Stop(void) {
    sensor.VL53L4CD_StopRanging();
    waitingForMeasurement = false;
    return ErrorCodes::OK;
}

ErrorCodes TofVL53L4CD::Continue(void) {
    sensor.VL53L4CD_StartRanging();
    waitingForMeasurement = false;
    timeoutOccured = false;
    ts_lastReadCall = millis();
    return ErrorCodes::OK;
}

#ifdef _MSC_VER
    #pragma endregion
    #pragma region TOF Class //--------------------------------------------------------------------------------------------------
#endif
void TofSensors::DisableAll(void) {
    leftBack.Off();
    leftFront.Off();
    rightFront.Off();
    rightBack.Off();
    frontUpper.Off();
    backUpper.Off();
    frontLower.Off();
    backLower.Off();
}

ErrorCodes TofSensors::Init(void) {
    DisableAll();
    EnableUpdate();
    delay(50);

    if (leftBack.Init() == true && leftFront.Init() == true && rightFront.Init() == true 
        && rightBack.Init() == true && backUpper.Init() == true && frontUpper.Init() == true
        && backLower.Init() == true /*&& frontLower.Init() == true*/)
        return ErrorCodes::OK;
    else 
        return ErrorCodes::ERROR;
}

ErrorCodes TofSensors::Update(void) {
    if (!updateEnabled) return ErrorCodes::OK;

    ErrorCodes errLB = leftBack.Read();
    ErrorCodes errLF = leftFront.Read();
    ErrorCodes errRF = rightFront.Read();
    ErrorCodes errRB = rightBack.Read();
    ErrorCodes errBU = backUpper.Read();
    ErrorCodes errFU = frontUpper.Read();
    ErrorCodes errBL = backLower.Read();
    // ErrorCodes errFL = frontLower.Read();

    if (errLB == ErrorCodes::NEW_DATA || errLF == ErrorCodes::NEW_DATA ||
        errRF == ErrorCodes::NEW_DATA || errRB == ErrorCodes::NEW_DATA ||
        errBU == ErrorCodes::NEW_DATA || errFU == ErrorCodes::NEW_DATA ||
        errBL == ErrorCodes::NEW_DATA /*|| errFL == ErrorCodes::NEW_DATA*/)
    {
        return ErrorCodes::NEW_DATA;
    }

    return ErrorCodes::NO_NEW_DATA;
}

void TofSensors::DisableUpdate(void) {
    updateEnabled = false;
}

void TofSensors::EnableUpdate(void) {
    updateEnabled = true;
}

uint16_t TofSensors::GetRange(TofType sensor) {
    EnableUpdate();

    switch (sensor)
    {
    case TofType::LEFT_BACK:
        return leftBack.GetRange();
        break;
    
    case TofType::LEFT_FRONT:
        return leftFront.GetRange();
        break;
    
    case TofType::RIGHT_FRONT:
        return rightFront.GetRange();
        break;

    case TofType::RIGHT_BACK:
        return rightBack.GetRange();
        break;

    case TofType::BACK_UPPER:
        return backUpper.GetRange();
        break;
    
    case TofType::FRONT_UPPER:
        return frontUpper.GetRange();
        break;

    case TofType::BACK_LOWER:
        return backLower.GetRange();
        break;
    
    case TofType::FRONT_LOWER:
        return frontLower.GetRange();
        break;
    
    default:
        return 0;
        break;
    }
}

TofStatus TofSensors::GetStatus(TofType sensor) {
    EnableUpdate();
    
    switch (sensor)
    {
    case TofType::LEFT_BACK:
        return leftBack.GetStatus();
        break;
    
    case TofType::LEFT_FRONT:
        return leftFront.GetStatus();
        break;
    
    case TofType::RIGHT_FRONT:
        return rightFront.GetStatus();
        break;

    case TofType::RIGHT_BACK:
        return rightBack.GetStatus();
        break;

    case TofType::BACK_UPPER:
        return backUpper.GetStatus();
        break;
    
    case TofType::FRONT_UPPER:
        return frontUpper.GetStatus();
        break;

    case TofType::BACK_LOWER:
        return backLower.GetStatus();
        break;
    
    case TofType::FRONT_LOWER:
        return frontLower.GetStatus();
        break;
    
    default:
        return TofStatus::TIMEOUT;
        break;
    }
}

bool TofSensors::IsDataNew(TofType sensor) {
    EnableUpdate();

    switch (sensor)
    {
    case TofType::LEFT_BACK:
        return leftBack.IsDataNew();
        break;
    
    case TofType::LEFT_FRONT:
        return leftFront.IsDataNew();
        break;
    
    case TofType::RIGHT_FRONT:
        return rightFront.IsDataNew();
        break;

    case TofType::RIGHT_BACK:
        return rightBack.IsDataNew();
        break;

    case TofType::BACK_UPPER:
        return backUpper.IsDataNew();
        break;
    
    case TofType::FRONT_UPPER:
        return frontUpper.IsDataNew();
        break;

    case TofType::BACK_LOWER:
        return backLower.IsDataNew();
        break;
    
    case TofType::FRONT_LOWER:
        return frontLower.IsDataNew();
        break;
    
    default:
        return false;
        break;
    }
}

bool TofSensors::AnyTimeoutOccured(void) {
    if (leftBack.TimeoutOccured() || leftFront.TimeoutOccured() || rightFront.TimeoutOccured() 
        || rightBack.TimeoutOccured() || frontUpper.TimeoutOccured() || backUpper.TimeoutOccured())
        return true;
    else 
        return false;
}

int8_t TofSensors::CalculateLeftRightError(float angleError, uint8_t sideWallThreshold, uint8_t gapRobotWall) {
	if (leftBack.GetRange() < sideWallThreshold && leftFront.GetRange() < sideWallThreshold && rightFront.GetRange() < sideWallThreshold && rightBack.GetRange() < sideWallThreshold)
		return ((leftBack.GetRange() + leftFront.GetRange()) / 2 - (rightFront.GetRange() + rightBack.GetRange()) / 2) / 2;
	else if (leftBack.GetRange() < sideWallThreshold && leftFront.GetRange() < sideWallThreshold)
		return (leftBack.GetRange() + leftFront.GetRange()) / 2 - gapRobotWall;
	else if (rightFront.GetRange() < sideWallThreshold && rightBack.GetRange() < sideWallThreshold)
		return gapRobotWall - (rightFront.GetRange() + rightBack.GetRange()) / 2;
	// else if (leftFront.GetRange() < sideWallThreshold && angleError >= -2.5 && angleError <= 2.5)
	// 	return leftFront.GetRange() - gapRobotWall;
	// else if (rightFront.GetRange() < sideWallThreshold && angleError >= -2.5 && angleError <= 2.5)
	// 	return gapRobotWall - rightFront.GetRange();
	else return 0;
}

uint8_t TofSensors::GetWalls(bool rampInfront, bool rampBehind) {
    uint8_t wallInfo = 0;

	if (leftBack.GetRange() < 150 
		&& leftFront.GetRange() < 150) wallInfo |= (1<<3);	//Left
	if (rightBack.GetRange() < 150
		&& rightFront.GetRange() < 150) wallInfo |= (1<<1);	//Right
	if (backUpper.GetRange() < 150 && !rampBehind) wallInfo |= (1<<2);	//Back
	if (frontUpper.GetRange() < 150 && !rampInfront) wallInfo |= (1<<0);	//Front

    #ifdef DEBUG_SCAN
        Serial.print("Wall Info: ");
        Serial.print(wallInfo, BIN);
        Serial.print("\tLB: ");
        Serial.print(leftBack.GetRange());
        Serial.print("\tLF: ");
        Serial.print(leftFront.GetRange());
        Serial.print("\tRF: ");
        Serial.print(rightFront.GetRange());
        Serial.print("\tRB: ");
        Serial.print(rightBack.GetRange());
        Serial.print("\tMF: ");
        Serial.print(frontUpper.GetRange());
        Serial.print("\tMB: ");
        Serial.println(backUpper.GetRange());
    #endif

	return wallInfo;
}

#ifdef _MSC_VER
    #pragma endregion
#endif