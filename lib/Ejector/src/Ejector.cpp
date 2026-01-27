/**
* @name:    Ejector.cpp
* @date:	16.01.2026
* @authors: Florian Wiesner, Paul Charusa
* @details: .cpp file for the ejector class
*/

// Libraries
#include "Ejector.h"

#ifdef _MSC_VER
    #pragma region Init //-------------------------------------------------------------------------------------------------------
#endif
void Ejector::Init(void) {
	pinMode(PIN_SERVO_LEFT, OUTPUT);
	pinMode(PIN_SERVO_RIGHT, OUTPUT);
	
	servoLeft.attach(PIN_SERVO_LEFT);
	servoRight.attach(PIN_SERVO_RIGHT);
	servoLeft.write(POS_CLOSED_LEFT);
	servoRight.write(POS_CLOSED_RIGHT);
}

#ifdef _MSC_VER
    #pragma endregion
    #pragma region Eject //------------------------------------------------------------------------------------------------------
#endif
ErrorCodes Ejector::Eject(bool side, uint8_t amount) {
	amount = constrain(amount, 1, 5);

	if (side == EJECT_LEFT) {
		for (uint8_t i = 0; i < amount; i++) {
			servoLeft.write(POS_OPEN_LEFT);
			delay(DELAY_OPEN);
			servoLeft.write(POS_CLOSED_LEFT);
			delay(DELAY_CLOSE);
		}
        return ErrorCodes::OK;
	}
	else if (side == EJECT_RIGHT) {
		for (uint8_t i = 0; i < amount; i++) {
			servoRight.write(POS_OPEN_RIGHT);
			delay(DELAY_OPEN);
			servoRight.write(POS_CLOSED_RIGHT);
			delay(DELAY_CLOSE);
		}
        return ErrorCodes::OK;
	}
    return ErrorCodes::UNKNOWN;
}