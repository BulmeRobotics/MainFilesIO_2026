//author: Vincent Rohkamm
//date: 18.11.2025
//description: User Interface for Robot

#include "UserInterface.h"

// Definitionen der statischen Member
GDTpoint_t UserInterface::LastContact{};
bool UserInterface::NewContact = false;

#ifdef _MSC_VER
  #pragma region Constructor //-----------------------------------------------------------
#endif

UserInterface::UserInterface(uint8_t updateInterval){
    UPDATE_INTERVAL = updateInterval;
}

void UserInterface::HandleMainMenu(){
    display.fillRoundRect(0, 6, 120, 468, 15, HL_COLOR);	//SecondaryBackground
	display.setTextSize(6);
	display.setTextColor(0x0000);

	//Display MenuButtons:
		//LocationMenu:
	display.fillRoundRect(10, 16, 100, 100, 15, BTN_COLOR);

	display.fillCircle(60, 46, 25, 0);
	display.fillTriangle(35, 46, 60, 106, 85, 46, 0);
	display.fillCircle(60, 46, 13, BTN_COLOR);

	//GeneralInfoMenu:
	display.fillRoundRect(10, 132, 100, 100, 15, BTN_COLOR);
	display.fillCircle(60, 182, 40, 0);
	display.fillCircle(60, 182, 35, BTN_COLOR);
	display.fillCircle(60, 182, 10, 0);
	display.fillCircle(60, 182, 30, BTN_COLOR);
	display.fillCircle(60, 182, 25, 0);
	display.fillCircle(60, 182, 20, BTN_COLOR);

	display.fillTriangle(10, 147, 110, 147, 60, 182, BTN_COLOR);
	display.fillTriangle(10, 217, 110, 217, 60, 182, BTN_COLOR);
	display.fillRect(25, 132, 70, 15, BTN_COLOR);
	display.fillRect(25, 217, 70, 15, BTN_COLOR);

	display.fillCircle(60, 182, 10, 0);
	display.fillCircle(60, 182, 5, BTN_COLOR);

	//SettingMenu:
	display.fillRoundRect(10, 248, 100, 100, 15, BTN_COLOR);
	display.fillCircle(60, 298, 33, 0);
	//display.fillCircle(60, 298, 15, BTN_COLOR);

	display.fillCircle(60, 268, 10, 0);	//Vertical
	display.fillCircle(60, 328, 10, 0);

	display.fillCircle(30, 298, 10, 0);	//Horizontal
	display.fillCircle(90, 298, 10, 0);

	display.fillCircle(81, 277, 10, 0);
	display.fillCircle(39, 277, 10, 0);

	display.fillCircle(81, 319, 10, 0);
	display.fillCircle(39, 319, 10, 0);
	display.fillCircle(60, 298, 19, BTN_COLOR);

	//InfoMenu:
	display.fillRoundRect(10, 364, 100, 100, 15, BTN_COLOR);
	display.setCursor(45, 393);
	display.fillCircle(60, 414, 38, 0);
	display.fillCircle(60, 414, 33, BTN_COLOR);
	display.print("I");

	display.setTextColor(TEXT_COLOR);
	display.setTextSize(4);

    //Menu switching handling
    if (NewContact == true){
		//Main Menu:
		if (LastContact.y <= 120 &&
            (*p_state == RobotState::SETTINGS || *p_state == RobotState::ABOUT ||
                *p_state == RobotState::INFO_SENSOR || *p_state == RobotState::INFO_VISUAL))
        {
			//Menu selector:
            //Location Information
            if (LastContact.x >= 360 && *p_state != RobotState::INFO_VISUAL)
                *p_state = RobotState::INFO_VISUAL;
            //Sensor Information
            else if (LastContact.x < 360 && LastContact.x >= 244 && *p_state != RobotState::INFO_SENSOR)
                *p_state = RobotState::INFO_SENSOR;
            //Settings
            else if (LastContact.x < 244 && LastContact.x >= 128 && *p_state != RobotState::SETTINGS)
                *p_state = RobotState::SETTINGS;
            //About
            else if (LastContact.x < 128 && *p_state != RobotState::ABOUT)
                *p_state = RobotState::ABOUT;

            BuzzerSignal(5,0,1);
		}
    }
}

#ifdef _MSC_VER
  #pragma endregion Constructor
  #pragma region Private Methods //----------------------------------------------------------------
#endif

    //Battery
uint8_t UserInterface::GetCharge(){
    float voltage = 0.2 + analogRead(batteryPin) * (3.216/1023.0) * 4.0;
    if(voltage > 8.2) lastPercent = 100;
    else if(voltage < 6.8) lastPercent = 0;
    else {
        lastPercent = 10 * voltage;
        lastPercent = 10 * map(lastPercent, 68, 82, 0, 10);
    }
    return lastPercent;
}

void UserInterface::DrawBattery(){
    display.fillRoundRect(680, 10, 100, 44, 5, HL_COLOR); //Battery Background
	display.setTextColor(TEXT_COLOR);
	//Battery symbol
	display.fillRoundRect(685, 24, 3, 16, 5, BG_COLOR);
	display.fillRoundRect(690, 15, 85, 34, 5, BG_COLOR);
	display.fillRoundRect(693, 18, 79, 28, 5, HL_COLOR);

	//Current Charge
	display.fillRoundRect(768 - (lastPercent * 0.75), 18, 4 + (lastPercent * 0.75), 28, 5, BTN_COLOR);
	display.setTextSize(2);
	display.setCursor(715, 25);
	char buff[4];
	sprintf(buff, "%3d", lastPercent);
	display.print(buff);
	display.setTextSize(TEXT_SIZE);
}

    // TOUCH HANDLER
void UserInterface::gigaTouchHandler(uint8_t contacts, GDTpoint_t* points) {
	NewContact = true;
	LastContact = points[0];
}

#ifdef _MSC_VER
  #pragma endregion Private Methods
  #pragma region Public Methods //------------------------------------------------------
#endif

void UserInterface::Initialize(){
    // Initialize NeoPixel
    pixels.begin();
    pixels.clear();

    //Initialize Buzzer and Signal-LED
    pinMode(buzzerPin, OUTPUT);
    pinMode(ledPin, OUTPUT);

    SetIllumination(0xFFFFFFFF); // Turn on Illumination at Startup

    //Initialize Display
    display.begin();
    display.setRotation(1);
    display.fillScreen(0x0000); // Black Screen

    //Display Boot Menu
    display.setTextColor(TEXT_COLOR);
	display.setTextSize(5);
	display.setCursor(10, 10);
	display.print("StartUp");

	//Display UI Version
	display.setTextSize(3);
	display.setCursor(574, 10);
	display.print("\tUI Ver. ");
	display.println(UI_VERSION);
	display.setTextSize(4);
	display.drawLine(0, 54, 800, 54, TEXT_COLOR);
	display.setCursor(15, 60);
	display.print("Display UI");
	display.setCursor(684, display.getCursorY());
	display.println("{OK}");

	//Start touch Detector
	if (touchDetector.begin()) {
		#ifdef DEBUG_UI
		Serial.print("Touch controller init - OK");
		#endif //
	}
	else {	//Touch failed
		#ifdef DEBUG_UI
		Serial.print("Touch controller init - FAILED");
		#endif // 
		AddInfoMsg("TouchDetection", "Failed", false);
	}
	touchDetector.onDetect(gigaTouchHandler);

    //Check Battery at Startup
    if(GetCharge() <= 10) {
        AddInfoMsg("Battery", "LOW/ERROR!", false);
        while(true){
            BuzzerSignal(200,200,1);
        }
    } else AddInfoMsg("Battery", "OK", true);
}
void UserInterface::ConnectPointer(RobotState* state, ColorSensing* cs){
    p_state = state;
    p_colorSens = cs;
    
    return;
}

void UserInterface::AddInfoMsg(String Info, String Message, bool success){
    if (p_state != nullptr && *p_state == RobotState::BOOT) display.setCursor(15, display.getCursorY());
    else if(p_state != nullptr && *p_state == RobotState::BT) display.setCursor(10, display.getCursorY());
	else if(p_state != nullptr && *p_state == RobotState::CHECKPOINT) display.setCursor(10, display.getCursorY());
    
    display.setTextSize(3);
	display.print(Info);
	display.setCursor(800 - (68 + 24 * Message.length()), display.getCursorY());
	
    if(success)	display.setTextColor(0x0fc0);
	else display.setTextColor(0xf800);

	display.print("[");
	display.setTextColor(TEXT_COLOR);
	display.print(Message);
	
    if(success)	display.setTextColor(0x0fc0);
	else display.setTextColor(0xf800);
	display.println("]");
    
	display.setTextColor(TEXT_COLOR);
}

void UserInterface::Update(){
    uint32_t currentMillis = millis();
    if(currentMillis >= lastUpdate + UPDATE_INTERVAL){
        lastUpdate = currentMillis;

        //Update Battery Status
        GetCharge();
        DrawBattery();

        //Draw and Handle mainMenu -> Menu Selector
        if (*p_state != RobotState::RUN) HandleMainMenu();

        // Handle Menus
        if(*p_state == RobotState::RUN){
            //RUN

        } else if (*p_state == RobotState::INFO_VISUAL){
            //Info Visual

        } else if (*p_state == RobotState::INFO_SENSOR){
            //Sensor Information

        } else if (*p_state == RobotState::SETTINGS){
            //Settings
            ConstructSettingsMenu();
            display.setTextSize(3);

            //Update driveMode
            display.setCursor(348, 20);
            display.setTextColor(TEXT_COLOR, HL_COLOR);
            switch (driveMode) {
            case ErrorCodes::straight:
                display.print("straight");
                break;
            case ErrorCodes::left:
                display.print("    left");
                break;
            case ErrorCodes::right:
                display.print("   right");
                break;
            case ErrorCodes::North:
                display.print("   North");
                break;
            case ErrorCodes::East:
                display.print("    East");
                break;
            case ErrorCodes::South:
                display.print("   South");
                break;
            case ErrorCodes::West:
                display.print("    West");
                break;
            default:
                display.print("   ERROR");
                break;
            }

            //Update speed:
            if (NewContact == true && LastContact.x >= 332 && LastContact.x <= 416 && millis() > lastChange + WAITTIME) {
                if (LastContact.y >= 150 && LastContact.y <= 355 && driveSpeed > 10){
                    driveSpeed -= 10;
                    signal.buzzer_pulse(5, 1);
                } 
                if (LastContact.y > 355 && LastContact.y < 500 && driveSpeed < 100) {
                    driveSpeed += 10;
                    signal.buzzer_pulse(5, 1);	
                } 
                lastChange = millis();
    #ifdef DEBUG_UI
                Serial.println("ChangeSPEED");
    #endif // DEBUG_UI
            }
            display.setCursor(354, 94);
            sprintf(buff, "%3d", driveSpeed);
            display.print(buff);

            //Update ColorSensor:
            if (NewContact == true && LastContact.x >= 205 && LastContact.x <= 285) {	//height 70, width 135; start y 200
                if (LastContact.y >= 160 && LastContact.y <= 295) {	//Black - x = 160...295
                    calibratingCS |= (0x01 | (1 << 6));
                    currentMenu = UI_Menu::CALIBRATE_CS;
                }
                if (NewContact == true && LastContact.y >= 315 && LastContact.y <= 450) {	//Blue - x = 315...450
                    calibratingCS |= (1 << 1 | (1 << 6));
                    currentMenu = UI_Menu::CALIBRATE_CS;
                }
                if (NewContact == true && LastContact.y >= 470 && LastContact.y <= 605) {	//Danger Zone x = 470...605
                    calibratingCS |= (1 << 2 | (1 << 6));
                    currentMenu = UI_Menu::CALIBRATE_CS;
                }
                if (NewContact == true && LastContact.y >= 625 && LastContact.y <= 760) {	//Checkpoint x = 625...760
                    calibratingCS |= (1 << 3 | (1 << 6));
                    currentMenu = UI_Menu::CALIBRATE_CS;
                }
            }
            if (NewContact == true && LastContact.x >= 332 && LastContact.x <= 416 && LastContact.y >= 500 && LastContact.y <= 635) {
                calibratingCS |= (1 << 4 | (1 << 6));
                currentMenu = UI_Menu::CALIBRATE_CS;
            }
            
            //Connect to BLE
            if(NewContact == true && LastContact.x >= 332 && LastContact.x <= 416 && LastContact.y >= 640){
                currentMenu = UI_Menu::BT_CONNECT;
                _FINISH_BLE_CONNECT = false;
                constructBLEInfo();
                NewContact = false;
                return;
            } 

            //Update drive into DZ
            if(	NewContact == true && millis() > (lastDZ_change + (WAITTIME * 5)) &&
                LastContact.y >= 150 && LastContact.x >= 110 && LastContact.x <= 190
            ) {
                *driveIntoDZ_UI = !*driveIntoDZ_UI;
                signal.buzzer_pulse(5,1);
                lastDZ_change = millis();
            }
            display.setTextColor(TEXT_COLOR, HL_COLOR);
            display.setCursor(150,318);
            display.print("Drive into DZ: ");
            if(*driveIntoDZ_UI) display.print("    true");
            else display.print("   false");

        } else if (LastContact.x < 128 && *p_state != RobotState::ABOUT){
            ConstructAboutMenu();
        }
    }
}

void UserInterface::ConstructAboutMenu(){
	//prepare font
	display.setTextSize(10);
	display.setTextColor(TEXT_COLOR);

	//Robot Name
	//display.fillRoundRect(130, 6, 400, 100, 20, BTN_COLOR);
	display.setCursor(150, 21);
	display.println("B.Robots");

	//HTL BULME Graz-G�sting
	display.setTextSize(TEXT_SIZE);
	display.setCursor(150, display.getCursorY());
	display.println("HTL Bulme Graz-Gosting");

	//UI-Version
	display.setTextSize(3);
	display.setCursor(150, display.getCursorY());
	display.print("\tUI Version ");
	display.println(UI_VERSION);

	//Team:
	display.setCursor(150, display.getCursorY() + 15);
	display.setTextSize(5);
	display.println("Team:");
	//Mentor
	display.setTextSize(TEXT_SIZE);
	display.setCursor(160, display.getCursorY() + 5);
	display.drawLine(150, display.getCursorY() - 4, 750, display.getCursorY() - 4, TEXT_COLOR);
	display.println("Mentor:");
	display.setTextSize(3);
	display.setCursor(175, display.getCursorY());
	display.println("Peter Frauscher");

	//Teammembers
	display.setCursor(160, display.getCursorY() + 5);
	display.setTextSize(TEXT_SIZE);
	display.println("Members:");
	display.setTextSize(3);
	display.setCursor(175, display.getCursorY());
	display.println("Paul Charusa | Florian Wiesner");
	display.setCursor(175, display.getCursorY());
	display.println("Thomas Rauch | Vincent Rohkamm");
}

void UserInterface::ConstructSettingsMenu(){
	//Drive Mode
	display.fillRoundRect(140, 10, 362, 44, 5, HL_COLOR);
	display.setTextSize(3);
	display.setTextColor(TEXT_COLOR, HL_COLOR);
	display.setCursor(150, 20);
	display.print("driveMode:");


	//speed
	display.fillRoundRect(140, 64, 340, 84, 5, HL_COLOR);	//eig 338 bei width
	display.setCursor(150, 94);
	display.print("speed:  -");
	display.setCursor(450, 94);
	display.print("+");

	//Caibrate CS - WHITE
	display.fillRoundRect(490, 64, 155, 84, 5, HL_COLOR);
	display.fillRoundRect(500, 71, 135, 70, 5, BTN_COLOR);

	display.setTextColor(TEXT_COLOR);
	display.setCursor(522, 98);
	display.print("WHITE");

	//Connect BLE (Bluetooth)
	display.fillRoundRect(655, 64, 125, 84, 5, HL_COLOR);
	display.fillRoundRect(665, 71, 105, 70, 5, BTN_COLOR);
	display.setCursor(690, 98);
	display.print("BLE");

	//Calibrate ColorSensors
	display.fillRoundRect(140, 158, 640, 122, 5, HL_COLOR);
	display.setCursor(150, 168);
	display.print("ColorSensor Calibration");
	display.fillRoundRect(160, 200, 135, 70, 5, BTN_COLOR);
	display.fillRoundRect(315, 200, 135, 70, 5, BTN_COLOR);
	display.fillRoundRect(470, 200, 135, 70, 5, BTN_COLOR);
	display.fillRoundRect(625, 200, 135, 70, 5, BTN_COLOR);

	display.setTextColor(0);
	display.setCursor(182, 223);
	display.print("Black");
	display.setTextColor(0x001f);
	display.setCursor(346, 223);
	display.print("Blue");
	display.setTextColor(0xf800);
	display.setCursor(483, 223);
	display.print("D-Zone");
	display.setTextColor(TEXT_COLOR);
	display.setCursor(638, 223);
	display.print("CheckP");

	display.setTextColor(TEXT_COLOR, HL_COLOR);
}

#ifdef _MSC_VER
  #pragma endregion Public Methods  
  #pragma region Signal Unit //-----------------------------------------------------------------------
#endif

void UserInterface::SetIllumination(uint32_t color){
    for(uint8_t i = 0; i < pixelNum; i++){
        pixels.setPixelColor(i, color);
    }
    if(color == 0) pixels.clear();
    pixels.show();
}

void UserInterface::BuzzerSignal(uint16_t time_ms, uint16_t pause_ms, uint8_t repetitions){
    for(uint8_t i = 0; i < repetitions; i++){
        digitalWrite(buzzerPin, HIGH);
        delay(time_ms);
        digitalWrite(buzzerPin, LOW);
        delay(pause_ms);
    }
}

void UserInterface::LEDSignal(uint16_t time_ms, uint16_t pause_ms, uint8_t repetitions){
    for(uint8_t i = 0; i < repetitions; i++){
        digitalWrite(ledPin, HIGH);
        delay(time_ms);
        digitalWrite(ledPin, LOW);
        delay(pause_ms);
    }
}

void UserInterface::LED_BUZZER_Signal(uint16_t time_ms, uint16_t pause_ms, uint8_t repetitions){
    for(uint8_t i = 0; i < repetitions; i++){
        digitalWrite(ledPin, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(time_ms);
        digitalWrite(ledPin, LOW);
        digitalWrite(buzzerPin, LOW);
        delay(pause_ms);
    }
}

#ifdef _MSC_VER
  #pragma endregion Signal Unit
#endif