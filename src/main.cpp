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

#define RESET_HEIGHT_SPAN 90
#define UPPER_LEVEL_HEIGHT 230
#define LOWER_LEVEL_HEIGHT -100



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
#include <Gyro.h>
#include <Motor.h>
#include <Ejector.h>
#include <Driving.h>
#include <Vcameras.h>

#ifdef _MSC_VER
  #pragma endregion Includes
  #pragma region Objects //------------------------------------------------------------------------
#endif

//Objects
UserInterface UI(100); // Update Interval: 50ms
EEPROM eeprom;
ColorSensing cs(&Serial);
Gyro gyro;
Ejector ejector;
TofSensors tof;
Mapping mapper;
Drivetrain drivetrain;
Driving robot;
Vcameras cameras;

#ifdef _MSC_VER
#pragma endregion Objects
#pragma region Variables //----------------------------------------------------------------------
#endif

//Variables
RobotState currentMenuState;
RunState currentRunState;
uint32_t lastButtonPressGray;

uint8_t speedMod = 1;

//Flags
bool _ROBOT_TURNING = false;
bool _RAMP_INFRONT = false;
bool _RAMP_BEHIND = false;

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
  serialSetup(BAUD_RATE);

  currentMenuState = RobotState::BOOT;

  Wire.begin();
  Wire.setClock(I2C_CLOCK);
  Wire1.begin();

  //Initialize Modules
    //User Interface
  UI.Initialize();

  Wire1.setClock(I2C_CLOCK);

  UI.ConnectPointer(&currentMenuState, &cs, &mapper, &cameras);
    //Buttons
  attachInterrupt(digitalPinToInterrupt(BUTTON_BLACK), ISR_BTN_BLACK, RISING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_GRAY), ISR_BTN_GRAY, RISING);
  lastButtonPressGray = millis();
  UI.AddInfoMsg("Buttons", "OK", true);


  //EEPROM
  eeprom.Init() != ErrorCodes::OK ? UI.AddInfoMsg("I2C", "ERROR", false) : UI.AddInfoMsg("I2C", "OK", true); 
  
  //Color sensor
  if(cs.Init(&Wire,&UI,&eeprom)!=0) UI.AddInfoMsg("Color Sensor", "ERROR", false);
  else UI.AddInfoMsg("Color Sensor", "OK", true);
  cs.EnableRead(true);

  //ToF
  if (tof.Init() == ErrorCodes::OK)
    UI.AddInfoMsg("TOF", "OK", true);
  else
    UI.AddInfoMsg("TOF", "ERROR", true);

  //Gyro
  if (gyro.Init() == ErrorCodes::OK)
    UI.AddInfoMsg("Gyro", "OK", true);
  else
    UI.AddInfoMsg("Gyro", "ERROR", true);

  //Ejector
  ejector.Init();
  UI.AddInfoMsg("Ejectors", "OK", true);
  UI.AddInfoMsg("Drivetrain", "OK", true);

  //Camera


  //Robot
  robot.init(&cs, &tof, &gyro, &mapper, &drivetrain);
  UI.AddInfoMsg("Driving", "OK", true);

  UI.AddInfoMsg("Finished STARTUP", "ACK", false);
#ifdef _MSC_VER
  #pragma endregion Initialization
  #pragma region Cyclic //-------------------------------------------------------------------------
#endif
while (true) {
  serialLoop();
  cyclicMainTask();

  if (currentMenuState == RobotState::RUN) {
    if (currentRunState == RunState::INITIAL) { //Initial Run Logic
      mapper.Reset();
      robot.enableBumpers();	//Enable Bumpers
			robot.startAlign();	//Start Aligning	
			gyro.ResetAllAngles();	//Gyro angle zero
			robot.maxRampIncline = 0;
			robot.currentRobotHeight = 0;
      currentRunState = RunState::SETTILE;
    }

    cyclicRunTask();  //Cyclic Run Tasks

    if (currentRunState == RunState::SETTILE) {
      UI.BuzzerSignal(5, 0, 1);
			mapper.SetTile(tof.GetWalls(_RAMP_INFRONT, _RAMP_BEHIND), cs.GetFloor());
			currentRunState = RunState::GET_INSTRUCTIONS;
			robot.lastSetTile = millis();
    } 

    else if (currentRunState == RunState::GET_INSTRUCTIONS) {
      UI.UpdateMap();
      //Get Instructions Logic
      switch (mapper.GetInstruction()) 
      {
      case Instructionset::T_North:
      //Turn North Logic
        robot.endDrive();
				robot.startAdjustment();
				currentRunState = RunState::TURN;
				robot.robotTargetAngle = Orientations::North;
        robot.startTurn(gyro.GetAngleFromOrientation(robot.robotTargetAngle));
				_ROBOT_TURNING = true;
        break;

      case Instructionset::T_East:
        //Turn East Logic
				robot.endDrive();
				robot.startAdjustment();
				currentRunState = RunState::TURN;
				robot.robotTargetAngle = Orientations::East;
        robot.startTurn(gyro.GetAngleFromOrientation(robot.robotTargetAngle));
				_ROBOT_TURNING = true;
        break;

      case Instructionset::T_South:
        //Turn South Logic
				robot.endDrive();
				robot.startAdjustment();
				currentRunState = RunState::TURN;
				robot.robotTargetAngle = Orientations::South;
        robot.startTurn(gyro.GetAngleFromOrientation(robot.robotTargetAngle));
				_ROBOT_TURNING = true;
        break;

      case Instructionset::T_West:
        //Turn West Logic
				robot.endDrive();
				robot.startAdjustment();
				currentRunState = RunState::TURN;
				robot.robotTargetAngle = Orientations::West;
        robot.startTurn(gyro.GetAngleFromOrientation(robot.robotTargetAngle));
				_ROBOT_TURNING = true;
        break;

      case Instructionset::D_Forward:
        //Drive Forward Logic
        //Blue Tile:
        if (cs.GetFloor() == PoI_Type::blue) {
          //Stoppen
          robot.endDrive();
          UI.Update();
          delay(5000);  //5 Sekunden warten
          //Weiterfahren
        }
        currentRunState = RunState::CHECK_DRIVE;	//Start Drive
        robot.startDrive();
        break;

      case Instructionset::MazeFinished:
        //Maze Finished Logic
        currentRunState = RunState::END;
        break;
      
      default:
        break;
      }

    } 
    
    else if (currentRunState == RunState::CHECKPOINT_RESET) {
      //Checkpoint Reset Logic
    } 
    
    else if (currentRunState == RunState::TURN) {
      //Turn Logic
      if (robot.controlTurn(gyro.GetAngleFromOrientation(robot.robotTargetAngle)) == ErrorCodes::TURNED) {
        robot.endTurn();
        _ROBOT_TURNING = false;
				currentRunState = RunState::SETTILE;
      }
    }
    
    else if (currentRunState == RunState::ALIGN) {
      //Align Logic
    } 
    
    else if (currentRunState == RunState::CHECK_DRIVE) {
      //Drive Logic
      if(robot.checkDrive() == ErrorCodes::CHECK_RAMP) currentRunState = RunState::RAMP;
			else currentRunState = RunState::SCAN;
    }

    else if (currentRunState == RunState::RAMP) {
      //Ramp Logic
      if(robot.rampHandler() == ErrorCodes::RAMP_END) currentRunState = RunState::SCAN;
			else currentRunState = RunState::DRIVE;
    }

    else if (currentRunState == RunState::DRIVE) {
      //Control Logic
      ErrorCodes driveSave = robot.controlDrive((UI.GetDriveSpeed() * speedMod), gyro.GetAngleFromOrientation(robot.robotTargetAngle));
			if(driveSave == ErrorCodes::CHECK_DRIVE) currentRunState = RunState::CHECK_DRIVE;
			else if (driveSave == ErrorCodes::TIMEOUT) {
        robot.timeoutDrive();
				currentRunState = RunState::SETTILE;
      }
			else currentRunState = RunState::RAMP;
    }

    else if (currentRunState == RunState::SCAN) {
			//Move the robot in the next tile and scan next field
			mapper.Move(true);	//Move robot forward
			//if Ramp detected during DRIVE give 
			// if(robot._ON_RAMP){
			// 	uint8_t rampLenght = 0;
			// 	int8_t rampDirection = 0;
				
			// 	//Calculate RAMP INFOS
			// 	rampLenght = (robot.RAMP_LENGTH / 300);	//Calculate num of Tiles
			// 	//Determine RAMP Direction
			// 	if(robot.currentRobotHeight < robot.currentRobotHeight + robot.RAMP_HEIGHT){
			// 		if(robot.currentRobotHeight <= LOWER_LEVEL_HEIGHT && robot.currentRobotHeight + robot.RAMP_HEIGHT >= UPPER_LEVEL_HEIGHT
			// 			) rampDirection = 2;
			// 		else if(robot.currentRobotHeight <= LOWER_LEVEL_HEIGHT && robot.currentRobotHeight + robot.RAMP_HEIGHT > LOWER_LEVEL_HEIGHT
			// 			) rampDirection = 1;
			// 		else if(robot.currentRobotHeight < UPPER_LEVEL_HEIGHT && robot.currentRobotHeight + robot.RAMP_HEIGHT >= UPPER_LEVEL_HEIGHT
			// 			) rampDirection = 1;						 
			// 	}
			// 	else if(robot.currentRobotHeight > robot.currentRobotHeight + robot.RAMP_HEIGHT){
			// 		if(robot.currentRobotHeight >= UPPER_LEVEL_HEIGHT && robot.currentRobotHeight + robot.RAMP_HEIGHT <= LOWER_LEVEL_HEIGHT
			// 			) rampDirection = -2;
			// 		else if(robot.currentRobotHeight >= UPPER_LEVEL_HEIGHT && robot.currentRobotHeight + robot.RAMP_HEIGHT < UPPER_LEVEL_HEIGHT
			// 			) rampDirection = -1;
			// 		else if(robot.currentRobotHeight < UPPER_LEVEL_HEIGHT && robot.currentRobotHeight + robot.RAMP_HEIGHT <= LOWER_LEVEL_HEIGHT
			// 			) rampDirection = -1;
			// 	}
			// 	robot.currentRobotHeight += robot.RAMP_HEIGHT;
			// 	if(robot.currentRobotHeight <= RESET_HEIGHT_SPAN && robot.currentRobotHeight >= -RESET_HEIGHT_SPAN) robot.currentRobotHeight = 0;
			// 	robot._ON_RAMP = false;
			// 	robot.maxRampIncline = 0;					
			// 	//pass RampInfos to Mapping
      //   mapper.Ramp(rampDirection, rampLenght);
			// }
			currentRunState = RunState::SETTILE;
    }

    else if (currentRunState == RunState::END) {
      //End of Run Logic
      robot.endDrive();
      robot.disableBumpers();
      UI.LED_BUZZER_Signal(100,200,3);
      currentMenuState = RobotState::ABOUT;
    }
  } 
  
  else if (currentMenuState == RobotState::SETTINGS) {
    //Settings Task
  } 
  
  else if (currentMenuState == RobotState::INFO_SENSOR) {

  } else if(currentMenuState == RobotState::INFO_VISUAL){

  } 
  
  else if (currentMenuState == RobotState::BT) {

  }

}
return 0;
}
#ifdef _MSC_VER
  #pragma endregion Cyclic
  #pragma region Functions //----------------------------------------------------------------------
#endif

void cyclicMainTask() {
  //Main cyclic tasks
  UI.Update();
  cs.Update();
}
void cyclicRunTask() {
  tof.Update();

  //Bumper Handling
	if(currentRunState != RunState::INITIAL){
		if(robot.bumperHandler() == ErrorCodes::BUMPER_WALL) currentRunState = RunState::SETTILE;
	}
}

void ISR_BTN_BLACK() {
	//Button for Starting and Checkpoint
  currentRunState = RunState::INITIAL;
  currentMenuState = RobotState::RUN;

}
void ISR_BTN_GRAY() {
  //Button for changing Drive Mode
	if (lastButtonPressGray + 300 < millis()) {
		UI.CycleDriveMode();
		lastButtonPressGray = millis();
	}
}

#ifdef VISUAL_STUDIO
  #pragma endregion Functions //-------------------------------------------------------------------
#endif