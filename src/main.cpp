/**
* @authors: Vincent Rohkamm, Florian Wiesner
* @date: 18.11.2025
* @description: Main Program file for Robot
*/

#ifdef _MSC_VER
  #pragma region Defines //------------------------------------------------------------------------
#endif

#define BAUD_RATE 115200
#define I2C_CLOCK 1000000UL
#define BUTTON_Black 49
#define BUTTON_Gray	 51



#ifdef _MSC_VER
  #pragma endregion Defines
  #pragma region Includes //-----------------------------------------------------------------------
#endif

//Includes
#include <Arduino.h>
#include <Wire.h>

  //Custom Includes - General
#include <SerialSetup.h>
#include <CustomDatatypes.h>

  //Custom Includes - Modules
#include <UserInterface.h>
#include <TofSensors.h>



#ifdef _MSC_VER
  #pragma endregion Includes
  #pragma region Objects //------------------------------------------------------------------------
#endif

//Objects
UserInterface UI(50);


#ifdef _MSC_VER
  #pragma endregion Objects
  #pragma region Variables //----------------------------------------------------------------------
#endif

//Variables
RobotState currentMenuState;

#ifdef _MSC_VER
  #pragma endregion Variables
  #pragma region Prototypes //----------------------------------------------------------------------
#endif

  //FUNKTIONEN
  void cyclicMainTask();
  void cyclicRunTask();

  void ISR_Btn_Black();
  void ISR_btn_Gray();


#ifdef _MSC_VER
  #pragma endregion Functions
  #pragma region Initialization //-----------------------------------------------------------------
#endif

int main(void) {
  currentMenuState = RobotState::BOOT;

  Serial.begin(BAUD_RATE);
  Wire.begin(I2C_CLOCK);
  Wire1.begin();

  //Initialize Modules
    //User Interface
  UI.Initialize();
  UI.ConnectPointer(&currentMenuState);
    //Buttons
  //attachInterrupt(digitalPinToInterrupt(BUTTON_Black), ISR_ButtonBlack, RISING);
	//attachInterrupt(digitalPinToInterrupt(BUTTON_Gray), ISR_ButtonGray, RISING);





  



#ifdef _MSC_VER
  #pragma endregion Initialization
  #pragma region Cyclic //-------------------------------------------------------------------------
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
#ifdef _MSC_VER
  #pragma endregion Cyclic
  #pragma region Functions //----------------------------------------------------------------------
#endif

void cyclicMainTask(){
  //Main cyclic tasks
  UI.Update();
}
void cyclicRunTask(){
  //Cyclic tasks when in RUN state
}

void ISR_ButtonBlack() {
	//Button for Starting and Checkpoint
}
void ISR_ButtonGray() {
  //Button for changing Drive Mode
	// if(lastButtonPressGray + 300 < millis()){
	// 	if (UI.driveMode != ErrorCodes::West) UI.driveMode = (ErrorCodes)((uint8_t)UI.driveMode + 1);
	// 	else UI.driveMode = ErrorCodes::straight;
	// 	(mapSys.changeDriveMode(UI.driveMode));
	// 	lastButtonPressGray = millis();
	// }
}

#ifdef VISUAL_STUDIO
  #pragma endregion Functions //-------------------------------------------------------------------
#endif