// author: Vincent Rohkamm
// date: 18.11.2025
// description: Main Program file for Robot

#ifdef VISUAL_STUDIO
  #pragma region Defines //--------------------------------------------
#endif

  #define BAUD_RATE 115200
  #define I2C_CLOCK 1000000UL



#ifdef VISUAL_STUDIO
  #pragma endregion Defines
  #pragma region Includes //-------------------------------------------
#endif

//INcludes
#include <Arduino.h>
#include <Wire.h>
#include "src/SerialSetup.h"


#ifdef VISUAL_STUDIO
  #pragma endregion Includes
  #pragma region Objects //--------------------------------------
#endif

//Objects


#ifdef VISUAL_STUDIO
  #pragma endregion Objects
  #pragma region Variables //------------------------------------
#endif

//Variables

#ifdef VISUAL_STUDIO
  #pragma endregion Variables
  #pragma region Prototypes //------------------------------------
#endif

  //FUNKTIONEN


#ifdef VISUAL_STUDIO
  #pragma endregion Functions
  #pragma region Initialization //--------------------------------
#endif

int main(void) {
  Serial.begin(BAUD_RATE);
  Wire.begin(I2C_CLOCK);
  Wire1.begin();
  


#ifdef VISUAL_STUDIO
  #pragma endregion Initialization
  #pragma region Cyclic //----------------------------------------
#endif



}
#ifdef VISUAL_STUDIO
  #pragma endregion Cyclic
  #pragma region Functions //------------------------------------
#endif

