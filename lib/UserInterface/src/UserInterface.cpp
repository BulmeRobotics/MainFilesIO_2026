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

#ifdef _MSC_VER

  #pragma endregion Constructor //-----------------------------------------------------------
  #pragma region Icon Drawing 
#endif

static void DrawIconLocation(GigaDisplay_GFX& display, uint16_t cx, uint16_t cy){
    display.fillCircle(cx, cy - 20, 25, 0);
    display.fillTriangle(cx - 25, cy - 20, cx, cy + 40, cx + 25, cy - 20, 0);
    display.fillCircle(cx, cy - 20, 13, BTN_COLOR);
}
static void DrawIconSensor(GigaDisplay_GFX& display, uint16_t cx, uint16_t cy){
    display.fillCircle(cx, cy, 40, 0);
    display.fillCircle(cx, cy, 35, BTN_COLOR);
    // (Weitere Ringe deines alten Codes hier optional ergänzen)
    display.fillCircle(cx, cy, 10, 0);
}
static void DrawIconSettings(GigaDisplay_GFX& display, uint16_t cx, uint16_t cy){

    display.fillCircle(cx, cy, 33, 0);

	display.fillCircle(cx, cy-30, 10, 0);	//Vertical
	display.fillCircle(cx, cy+30, 10, 0);

	display.fillCircle(cx-30, cy, 10, 0);	//Horizontal
	display.fillCircle(cx+30, cy, 10, 0);

	display.fillCircle(cx+21, cy-21, 10, 0);
	display.fillCircle(cx+9, cy-21, 10, 0);

	display.fillCircle(cx+21, cy+21, 10, 0);
	display.fillCircle(cx+9, cy+21, 10, 0);
	display.fillCircle(cx, cy, 19, BTN_COLOR);
}
static void DrawIconAbout(GigaDisplay_GFX& display, uint16_t cx, uint16_t cy){
    display.fillCircle(cx, cy, 38, 0);
    display.fillCircle(cx, cy, 33, BTN_COLOR);
    display.setCursor(cx - 10, cy - 15);
    display.setTextSize(4);
    display.setTextColor(TEXT_COLOR);
    display.print("I");
}

#ifdef _MSC_VER
  #pragma endregion Icon Drawing //-----------------------------------------------------------
  #pragma region MainMenu 
#endif

// ------------------------------------------------------------------
// HAUPTMENÜ (LINKER BALKEN)
// ------------------------------------------------------------------

void UserInterface::DrawMainMenuStatic() {
    // Grauer Hintergrundbalken
    display.fillRoundRect(0, 6, 120, 468, 15, HL_COLOR);

    // Buttons zeichnen (Die Klasse übernimmt Form, Text und Icons!)
    btnMenuLocation.Draw(display);
    btnMenuSensor.Draw(display);
    btnMenuSettings.Draw(display);
    btnMenuAbout.Draw(display);
}

void UserInterface::HandleMainMenu(uint16_t tx, uint16_t ty) {
    if (btnMenuLocation.IsPressed(tx, ty) && *p_state != RobotState::INFO_VISUAL) {
        *p_state = RobotState::INFO_VISUAL;
        BuzzerSignal(5, 0, 1);
    }
    else if (btnMenuSensor.IsPressed(tx, ty) && *p_state != RobotState::INFO_SENSOR) {
        *p_state = RobotState::INFO_SENSOR;
        BuzzerSignal(5, 0, 1);
    }
    else if (btnMenuSettings.IsPressed(tx, ty) && *p_state != RobotState::SETTINGS) {
        *p_state = RobotState::SETTINGS;
        BuzzerSignal(5, 0, 1);
    }
    else if (btnMenuAbout.IsPressed(tx, ty) && *p_state != RobotState::ABOUT) {
        *p_state = RobotState::ABOUT;
        BuzzerSignal(5, 0, 1);
    }
}

#ifdef _MSC_VER
  #pragma endregion MainMenu //-----------------------------------------------------------
  #pragma region Settings 
#endif

// ------------------------------------------------------------------
// SETTINGS
// ------------------------------------------------------------------

void UserInterface::ConstructSettingsMenu() {
    display.fillScreen(BG_COLOR); // Komplettes Display löschen
    DrawMainMenuStatic();         // Linke Navigation malen

    // Label-Boxen (Statischer Text)
    display.fillRoundRect(140, 10, 362, 44, 5, HL_COLOR);
    display.fillRoundRect(140, 158, 640, 122, 5, HL_COLOR);
    
    display.setTextSize(3);
    display.setTextColor(TEXT_COLOR, HL_COLOR);
    display.setCursor(150, 20);
    display.print("driveMode:");
    display.setCursor(150, 168);
    display.print("ColorSensor Calibration");

    // Alle Buttons malen (Der Text wird durch die Klasse automatisch zentriert!)
    btnSpeedMinus.Draw(display, "speed:  -");
    btnSpeedPlus.Draw(display, "+");
    btnCalibWhite.Draw(display, "WHITE");
    btnBleConnect.Draw(display, "BLE");
    
    btnCalibBlack.Draw(display, "Black");
    btnCalibBlue.Draw(display, "Blue");
    btnCalibDZone.Draw(display, "D-Zone");
    btnCalibCheckP.Draw(display, "CheckP");
}


#ifdef _MSC_VER
  #pragma endregion Settings
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

bool UserInterface::GetValidTouch(uint16_t &touchX, uint16_t &touchY){
    if (NewContact) {
        NewContact = false;
        
        uint32_t time = millis();
        if (time - lastTouch >= touchDebounce) {
            lastTouch = time;
            touchX = LastContact.x;
            touchY = LastContact.y;
            return true;
        }
    }
    return false;
}

#ifdef _MSC_VER
  #pragma endregion Private Methods
  #pragma region Calibration //------------------------------------------------------
#endif

void UserInterface::ShowCalibrationScreen(PoI_Type type){
    uint16_t popX = 150, popY = 150, popW = 500, popH = 180;
    display.fillRoundRect(popX + 5, popY + 5, popW, popH, 15, 0x0000); 
    display.fillRoundRect(popX, popY, popW, popH, 15, BG_COLOR);
    display.drawRoundRect(popX, popY, popW, popH, 15, HL_COLOR);
    display.drawRoundRect(popX+1, popY+1, popW-2, popH-2, 15, HL_COLOR); // Dickerer Rahmen
    
    // Titel schreiben
    display.setTextColor(TEXT_COLOR);
    display.setTextSize(4);
    display.setCursor(popX + 20, popY + 20);
    display.print("Calibrating ");
    
    // Name der Farbe auflösen
    switch(type) {
        case PoI_Type::black:      display.print("BLACK"); break;
        case PoI_Type::blue:       display.print("BLUE"); break;
        case PoI_Type::white:      display.print("WHITE"); break;
        case PoI_Type::dangerZone: display.print("D-ZONE"); break;
        case PoI_Type::red:        display.print("RED"); break;
        case PoI_Type::checkpoint: display.print("CHECKP"); break;
        default:                   display.print("UNKNOWN"); break;
    }
    
    // Trennlinie
    display.drawLine(popX, popY + 60, popX + popW, popY + 60, HL_COLOR);
    
    // Initiale Progress-Anzeige
    display.setTextSize(3);
    display.setCursor(popX + 20, popY + 80);
    display.print("Progress:   0%");
}
void UserInterface::UpdateCalibrationProgress(uint8_t step, uint8_t totalSteps){
    uint16_t popX = 150, popY = 150;
    
    // Prozentzahl überschreiben (Dank BG_COLOR schmiert der Text nicht!)
    display.setTextColor(TEXT_COLOR, BG_COLOR); 
    display.setTextSize(3);
    display.setCursor(popX + 182, popY + 80);
    
    uint8_t percent = (step * 100) / totalSteps;
    char buff[5];
    sprintf(buff, "%3d%%", percent);
    display.print(buff);
    
    // Dynamischen Fortschrittsbalken zeichnen
    display.fillRoundRect(popX + 20, popY + 130, (460 * step) / totalSteps, 20, 5, BTN_COLOR);
}
void UserInterface::FinishCalibration(bool success){
    uint16_t popX = 150, popY = 150;
    
    // Status-Text in Grün (Erfolg) oder Rot (Fehler) überschreiben
    display.setTextColor(success ? 0x07E0 : 0xF800, BG_COLOR); 
    display.setTextSize(3);
    display.setCursor(popX + 20, popY + 80);
    display.print(success ? "Done! Touch to close.   " : "ERROR! Touch to close.  ");
    
    // --- Warten: 3 Sekunden oder Touch ---
    uint32_t start = millis();
    NewContact = false; // Touch-Flag sicherheitshalber resetten
    
    while (millis() - start < 3000) {
        if (NewContact) {
            NewContact = false;
            BuzzerSignal(5, 0, 1);
            break; // Beendet das Warten sofort bei Touch
        }
        delay(10); // Verhindert Watchdog-Crashes
    }
    
    // Wenn alles vorbei ist, zwingen wir das UI, das Settings-Menü neu zu zeichnen
    *p_state = RobotState::SETTINGS;
    lastState = RobotState::CALIBRATION;
}

#ifdef _MSC_VER
  #pragma endregion Calibration
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
void UserInterface::ConnectPointer(RobotState* state, ColorSensing* cs, Mapping* mapping){
    p_state = state;
    p_colorSens = cs;
    p_mapping = mapping;

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
    uint32_t time = millis();
    if (time < lastUpdate + UPDATE_INTERVAL) return;

    lastUpdate = time;

    //Toch Handling
    uint16_t tx = 0, ty = 0;
    bool touched = GetValidTouch(tx, ty);

    //Rendering
    if(*p_state != lastState){
        switch (*p_state) {
        case RobotState::SETTINGS:
            ConstructSettingsMenu();
            break;
        case RobotState::ABOUT:
            ConstructAboutMenu();
            break;

        
        default:
            break;
        }
        lastState = *p_state;
    }

    // Update Battery Status
    if(*p_state == RobotState::SETTINGS || *p_state == RobotState::ABOUT || *p_state == RobotState::INFO_SENSOR || *p_state == RobotState::INFO_VISUAL){
        GetCharge();
        DrawBattery();
    }
    

    //Draw and Handle mainMenu -> Menu Selector
    if (touched && (*p_state == RobotState::SETTINGS || *p_state == RobotState::ABOUT || *p_state == RobotState::INFO_SENSOR || *p_state == RobotState::INFO_VISUAL))
        HandleMainMenu(tx,ty);
    
    // Handle Menus
    if(*p_state == RobotState::RUN){
        //RUN

    } else if (*p_state == RobotState::INFO_VISUAL){
        //Info Visual

    } else if (*p_state == RobotState::INFO_SENSOR){
        //Sensor Information

    } else if (*p_state == RobotState::SETTINGS){
        //Settings

        //Update driveMode
        display.setCursor(348, 20);
        display.setTextColor(TEXT_COLOR, HL_COLOR);
        display.setTextSize(3);
        switch (driveMode) {
        case ErrorCodes::straight:
            display.print("straight");
            break;
        case ErrorCodes::north:
            display.print("   North");
            break;
        case ErrorCodes::east:
            display.print("    East");
            break;
        case ErrorCodes::south:
            display.print("   South");
            break;
        case ErrorCodes::west:
            display.print("    West");
            break;
        default:
            display.print("   ERROR");
            break;
        }

        //Update speed:
        display.setCursor(354, 94);
        char buff[6];
        sprintf(buff, "%3d", driveSpeed);
        display.print(buff);

        // Update Buttons
        if(touched){
            //Speed
            if(btnSpeedMinus.IsPressed(tx,ty) && driveSpeed > 10){
                driveSpeed -= 10;
                BuzzerSignal(5, 0, 1);
            } else if(btnSpeedPlus.IsPressed(tx,ty) && driveSpeed < 100){
                driveSpeed += 10;
                BuzzerSignal(5, 0, 1);
            }

            //Calibration
            else if(btnCalibWhite.IsPressed(tx,ty)){
                *p_state = RobotState::CALIBRATION;
                p_colorSens->Calibrate(PoI_Type::white);
                BuzzerSignal(5, 0, 1);
            }
            else if(btnCalibBlack.IsPressed(tx,ty)){
                *p_state = RobotState::CALIBRATION;
                p_colorSens->Calibrate(PoI_Type::black);
                BuzzerSignal(5, 0, 1);
            }
            else if(btnCalibBlue.IsPressed(tx,ty)){
                *p_state = RobotState::CALIBRATION;
                p_colorSens->Calibrate(PoI_Type::blue);
                BuzzerSignal(5, 0, 1);
            }
            else if(btnCalibDZone.IsPressed(tx,ty)){
                *p_state = RobotState::CALIBRATION;
                p_colorSens->Calibrate(PoI_Type::red);
                BuzzerSignal(5, 0, 1);
            }
            else if(btnCalibCheckP.IsPressed(tx,ty)){
                *p_state = RobotState::CALIBRATION;
                p_colorSens->Calibrate(PoI_Type::checkpoint);
                BuzzerSignal(5, 0, 1);
            }
            //BLE
            RobotState::BT;
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

ErrorCodes UserInterface::CycleDriveMode(){
    if (driveMode != ErrorCodes::west) driveMode = (ErrorCodes)((uint8_t)driveMode + 1);
	else driveMode = ErrorCodes::straight;
	p_mapping->SetPriority(driveMode);
}

void UserInterface::ConnectPointer(RobotState* state, ColorSensing* cs, Mapping* mapping){
    p_state = state;
    p_colorSens = cs;
    p_mapping = mapping;
    return;
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