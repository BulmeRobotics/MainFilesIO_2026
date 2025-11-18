/*
 Name:		SerialSetup.cpp
 Created:	15.12.2023
 Author:	Florian Wiesner

 Short Describtion:
 .cpp file with the functions for Initialisation and Setup
 for the Serial Monitor. This way, because the Serial Monitor
 on Arduino GIGA R1 doesn't work by defaut with main() - structure
*/
//-----------------------------------------------------------------------------------------

//Libraries
#include "SerialSetup.h"
#include <USB/PluggableUSBSerial.h>
#include <Arduino.h>

//Definitons
#undef main

//-----------------------------------------------------------------------------------------
//Function Definitions
//Function to initialize the Serial monitor for the Arduino GIGA R1
void serialSetup(int baudrate_f)
{
	init();
	initVariant();

#if defined(SERIAL_CDC)
	PluggableUSBD().begin();
	_SerialUSB.begin(baudrate_f);
#endif

	Serial.begin(baudrate_f);
	Serial2.begin(baudrate_f);
	Serial3.begin(baudrate_f);
}

//Function that calls the SerialEventRunner every cycle of the loop
void serialLoop(void)
{
	if (arduino::serialEventRun)
	{
		arduino::serialEventRun();
	}
}