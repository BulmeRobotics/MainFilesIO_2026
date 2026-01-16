/**
* @name:    Battery.cpp
* @date:	15.01.2026
* @authors: Florian Wiesner, Paul Charusa
* @details: .cpp file for the Battery
*/

// Libraries
#include "Battery.h"

#ifdef _MSC_VER
    #pragma region Constructors //-----------------------------------------------------------------------------------------------
#endif
Battery::Battery(uint32_t updateIntervalTime, UserInterface* pointerUI) {
    intervalTime = updateIntervalTime;
    lastUpdate = millis();
    p_UI = pointerUI;
}
Battery::Battery(){
    intervalTime = 5000; // Default to 5 seconds
    lastUpdate = millis();
}

#ifdef _MSC_VER
    #pragma endregion
    #pragma region Basic methods //----------------------------------------------------------------------------------------------
#endif
float Battery::GetBatteryVoltage(void) {
	int per = analogRead(A0);
	float voltage = (per * (3.216 / 1024.0));
	voltage = voltage * 4.0;

	return voltage + 0.2;	//Offset for the voltage divider
}

uint8_t Battery::GetBatteryPercent(void) {
	float voltage = GetBatteryVoltage();
	int percent;

	if (voltage >= 8.2)	percent = 100;
	else if (voltage < 6.8)	percent = 0;
	else {
		int voltage_per = voltage * 10;
		percent = map(voltage_per, 68, 82, 0, 10);
		percent *= 10;
	}

	//if ((percent <= (last_per - 10)) || (percent >= (last_per + 10))) last_per = percent;
	last_per = percent;
	return last_per;
}

#ifdef _MSC_VER
    #pragma endregion
    #pragma region UI methods //-------------------------------------------------------------------------------------------------
#endif
// ErrorCodes Battery::BatteryUpdateHandler(){
// 	if(millis() >= (lastUpdate + intervalTime)){
// 		lastUpdate = millis();

// 		p_UI->batteryChargePercent = GetBatteryPercent(); //Update Battery percentage
// 		p_UI->updateBattery = true; //Update UI
// 	}
// 	return ErrorCodes::OK;
// }

// ErrorCodes Battery::InitScreen(){
// 	//Configure Battery
// 	if (GetBatteryVoltage() > 6.8) p_UI->addStartUpInfo("Battery", "OK", true);
// 	else p_UI->addStartUpInfo("Battery", "LOW/ERROR", false);
// 	p_UI->batteryChargePercent = getBatteryPer();
// 	p_UI->updateBattery = true; //Update UI

// 	lastUpdate = millis();
	
// 	if (p_UI->batteryChargePercent < 10) {
// 		return ErrorCodes::BatteryLOW;
// 	}
// 	else return ErrorCodes::OK;
// }
#ifdef _MSC_VER
    #pragma endregion
#endif

