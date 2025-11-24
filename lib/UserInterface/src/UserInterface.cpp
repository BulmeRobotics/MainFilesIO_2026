//author: Vincent Rohkamm
//date: 18.11.2025
//description: User Interface for Robot

#include "UserInterface.h"

#ifdef _MSC_VER
  #pragma region Constructor //-----------------------------------------------------------
#endif

UserInterface::UserInterface(uint8_t updateInterval){
    UPDATE_INTERVAL = updateInterval;
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

void UserInterface::AddInfoMsg(String Info, String Message, bool success){
    if (statePointer != nullptr && *statePointer == RobotState::BOOT) display.setCursor(15, display.getCursorY());
    else if(statePointer != nullptr && *statePointer == RobotState::BT) display.setCursor(10, display.getCursorY());
	else if(statePointer != nullptr && *statePointer == RobotState::CHECKPOINT) display.setCursor(10, display.getCursorY());
    
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
    }
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