//author: Vincent Rohkamm
//date: 18.11.2025
//description: User Interface for Robot

#include "UserInterface.h"

#ifdef _MSC_VER
  #pragma region Constructor //-----------------------------------------------------------
#endif

UserInterface::UserInterface(){

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

    // TOUCH HANDLER

#ifdef _MSC_VER
  #pragma endregion Private Methods
  #pragma region UserInterface Global Methods //------------------------------------------------------
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

#ifdef _MSC_VER
  #pragma endregion UserInterface Global Methods  
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