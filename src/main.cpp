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

//Includes
#include <Arduino.h>
#include <Wire.h>

  //Custom Includes - General
#include <SerialSetup.h>
#include <CustomDatatypes.h>

  //Custom Includes - Modules



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
RobotState currentMenuState;

#ifdef VISUAL_STUDIO
  #pragma endregion Variables
  #pragma region Prototypes //------------------------------------
#endif

  //FUNKTIONEN
  void cyclicMainTask();
  void cyclicRunTask();


#ifdef VISUAL_STUDIO
  #pragma endregion Functions
  #pragma region Initialization //--------------------------------
#endif

int main(void) {
  currentMenuState = RobotState::BOOT;

  Serial.begin(BAUD_RATE);
  Wire.begin(I2C_CLOCK);
  Wire1.begin();



#ifdef VISUAL_STUDIO
  #pragma endregion Initialization
  #pragma region Cyclic //----------------------------------------
#endif
currentMenuState = RobotState::SETTINGS;
while(true){
  cyclicMainTask();

  if(currentMenuState == RobotState::RUN){
    cyclicRunTask();

  } else if(currentMenuState == RobotState::SETTINGS){
    //Settings Task
  } else if(currentMenuState == RobotState::INFO_SENSOR){

  } else if(currentMenuState == RobotState::INFO_VISUAL){

  } else if(currentMenuState == RobotState::CALIBRATION){

  } else if(currentMenuState == RobotState::BT){

  }
} return 0;}
#ifdef VISUAL_STUDIO
  #pragma endregion Cyclic
  #pragma region Functions //------------------------------------
#endif

void cyclicMainTask(){
  //Main cyclic tasks
}
void cyclicRunTask(){
  //Cyclic tasks when in RUN state
}