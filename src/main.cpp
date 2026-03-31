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
#define BUTTON_BLACK 49
#define BUTTON_GRAY	 51



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

#include <Mapping.h>

  //Custom Includes - Modules
#include <UserInterface.h>
#include <TofSensors.h>
#include <ColorSensing.h>


#ifdef _MSC_VER
  #pragma endregion Includes
  #pragma region Objects //------------------------------------------------------------------------
#endif

//Objects
UserInterface UI(50); // Update Interval: 50ms
EEPROM eeprom;
ColorSensing cs(&Serial);
Mapping mapper;

#ifdef _MSC_VER
#pragma endregion Objects
#pragma region Variables //----------------------------------------------------------------------
#endif

//Variables
RobotState currentMenuState;
RunState currentRunState;

uint32_t lastButtonPressGray;

#ifdef _MSC_VER
  #pragma endregion Variables
  #pragma region Prototypes //----------------------------------------------------------------------
#endif

  //FUNKTIONEN
  void cyclicMainTask();
  void cyclicRunTask();

  void ISR_BTN_BLACK();
  void ISR_BTN_GRAY();


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
  UI.ConnectPointer(&currentMenuState, &cs, &mapper);
    //Buttons
  attachInterrupt(digitalPinToInterrupt(BUTTON_BLACK), ISR_BTN_BLACK, RISING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_GRAY), ISR_BTN_GRAY, RISING);
  lastButtonPressGray = millis();

  UI.AddInfoMsg("System", "OK", true);


  if(cs.Init(&Wire,&UI,&eeprom)!=0) UI.AddInfoMsg("Color Sensor", "ERROR", false);
  else UI.AddInfoMsg("Color Sensor", "OK", true);
  cs.EnableRead(true);

  //ROBOT (Driving, ToF)

  //Gyro

  //Ejector

  //MotorInterrupts

  //Camera

  UI.AddInfoMsg("Finished STARTUP", "ACK", false);
#ifdef _MSC_VER
  #pragma endregion Initialization
  #pragma region Cyclic //-------------------------------------------------------------------------
#endif
currentMenuState = RobotState::SETTINGS;
while(true){
  cyclicMainTask();

  if(currentMenuState == RobotState::RUN){
    if(currentRunState == RunState::INITIAL){ //Initial Run Logic
      mapper.Reset();
      currentRunState = RunState::SETTILE;
    }

    cyclicRunTask();  //Cyclic Run Tasks

    if(currentRunState == RunState::SETTILE){
      //SetTile Logic

      mapper.SetTile(/*walls*/0, /*floor*/TileType::visited);
    } else if(currentRunState == RunState::GET_INSTRUCTIONS){
      //Get Instructions Logic
      switch (mapper.GetInstruction()) 
      {
      case Instructionset::T_North:
        //Turn North Logic
        break;
      case Instructionset::T_East:
        //Turn East Logic
        break;
      case Instructionset::T_South:
        //Turn South Logic
        break;
      case Instructionset::T_West:
        //Turn West Logic
        break;
      case Instructionset::D_Forward:
        //Drive Forward Logic

        //Blue Tile:
        if(cs.GetFloor() == PoI_Type::blue){
          //Stoppen
          delay(5000);  //5 Sekunden warten
          //Weiterfahren
        }
        break;
      case Instructionset::MazeFinished:
        //Maze Finished Logic
        currentRunState = RunState::END;
        break;
      
      default:
        break;
      }

    } else if(currentRunState == RunState::CHECKPOINT_RESET){
      //Checkpoint Reset Logic
    } else if(currentRunState == RunState::END){
      //End of Run Logic
      UI.LED_BUZZER_Signal(100,200,3);
    } else if(currentRunState == RunState::TURN){
      //Turn Logic
    } else if(currentRunState == RunState::ALIGN){
      //Align Logic
    } else if(currentRunState == RunState::DRIVE){
      //Drive Logic

      
    }



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
  cs.Update();
}
void cyclicRunTask(){
  //Cyclic tasks when in RUN state
}

void ISR_BTN_BLACK() {
	//Button for Starting and Checkpoint
  currentRunState = RunState::INITIAL;
  currentMenuState = RobotState::RUN;

}
void ISR_BTN_GRAY() {
  //Button for changing Drive Mode
	if(lastButtonPressGray + 300 < millis()){
		UI.CycleDriveMode();
		lastButtonPressGray = millis();
	}

  
}

#ifdef VISUAL_STUDIO
  #pragma endregion Functions //-------------------------------------------------------------------
#endif