#pragma once
/*
 Name:		SerialSetup.h
 Created:	15.12.2023
 Author:	Florian Wiesner

 Short Describtion:
 Header file for Initialisation and Setup for the Serial Monitor.
 This way, because the Serial Monitor on Arduino GIGA R1 doesn't
 work by defaut with main() - structure
*/
//-----------------------------------------------------------------------------------------

//Libraries
#include <Arduino.h>
#include <USB/PluggableUSBSerial.h>

//Definitons
#undef main

//Function Prototypes
void serialSetup(int baudrate_f);
void serialLoop(void);
int atexit(void (* /*func*/)()) { return 0; }
void initVariant() __attribute__((weak));
void initVariant() { }
void setupUSB() __attribute__((weak));
void setupUSB() { }