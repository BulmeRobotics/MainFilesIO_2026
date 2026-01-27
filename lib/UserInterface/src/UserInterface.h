#pragma once

//author: Vincent Rohkamm
//date: 18.11.2025
//description: User Interface for Robot

#ifdef _MSC_VER
    #pragma region Includes //-----------------------------------------------------------------------
#endif

#include <Arduino.h>
#include <Arduino_GigaDisplay.h>
#include <Arduino_GigaDisplay_GFX.h>
#include <Arduino_GigaDisplayTouch.h>
#include <Adafruit_NeoPixel.h>

#include <CustomDatatypes.h>
#include <ColorSensing.h>

#ifdef _MSC_VER
    #pragma endregion Includes
    #pragma region UI Class//----------------------------------------------------------------
#endif

class UserInterface {
private:
    const char UI_VERSION[4] = "0.1";

    #define TEXT_SIZE 4
    #define TEXT_COLOR 0xce4a
    #define BG_COLOR 0
    #define HL_COLOR 0x1168
    #define BTN_COLOR 0x630c

    // Display Objects
    GigaDisplay_GFX display;
    Arduino_GigaDisplayTouch touchDetector;
    GigaDisplayRGB rgb;

    static void gigaTouchHandler(uint8_t contacts, GDTpoint_t* points);	//Touch detection INTERRUPT
    static GDTpoint_t LastContact;
    static bool NewContact;

    // LEDs and Buzzer
    const uint8_t neoPin = 8;
    const uint8_t pixelNum = 18;
    const uint8_t ledPin = 56;
    const uint8_t buzzerPin = 58;
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(pixelNum, neoPin, NEO_GRBW + NEO_KHZ800);

    // Battery Measurement
    const uint8_t batteryPin = A0;
    uint8_t lastPercent = 0;

    // Settings
    uint8_t UPDATE_INTERVAL = 50; //in ms
    uint32_t lastUpdate = 0;
    
    // Pointers
    RobotState* p_state = nullptr;
    ColorSensing* p_colorSens = nullptr;

    // Private Methods

    // @brief Draws menu buttons
    void HandleMainMenu();

    // @brief Draws Battery Status on Display
    void DrawBattery();



public:

    /**
     * @brief Construct a new User Interface object
     * @param updateInterval Update Interval for the User Interface in milliseconds
     */
    UserInterface(uint8_t updateInterval);    //CONSTRUCTOR

    /**
     * @brief Updates the User Interface (should be called cyclically)
     */
    void Update();

    // EVENTS ---------------------------------------------------------------------------------

    void Initialize();

    /**
     * @brief Connects the other classes to the User Interface
     */
    void ConnectPointer(RobotState* state, ColorSensing* cs);

    /**
     * @brief Adds an Information to the Message Log in startup or BLE screen
     * @param Info Title of the Message
     * @param Message Detailed Status (OK / FAIL / etc.)
     * @param success If true, Message is shown in green, else in red
     */
    void AddInfoMsg(String Info, String Message, bool success);

    // POP-Ups / Error Messages ---------------------------------------------------------------------------------

    /**
     * @brief Shows an Error Message on the Display
     * @param error Error Code to Show
     */
    void ShowErrorMsg(ErrorCodes error);

    /**
     * @brief Shows an Error Message on the Display with a custom message
     * @param error Error Code to Show
     * @param message Custom message to display
     */
    void ShowErrorMsg(ErrorCodes error, String message);

    // SIGNALS ---------------------------------------------------------------------------------

    /** 
     * @brief Triggers the Buzzer Signal; is Blocking!
     * @param time_ms Duration of the signal in milliseconds
     * @param pause_ms Pause between signals in milliseconds
     * @param repetitions Number of repetitions of the signal
     */
    void BuzzerSignal(uint16_t time_ms, uint16_t pause_ms, uint8_t repetitions);

    /**
     * @brief Triggers the LED Signal; is Blocking!
     * @param color Color of the LED signal (0xRRGGBBWW)
     * @param time_ms Duration of the signal in milliseconds
     * @param pause_ms Pause between signals in milliseconds
     * @param repetitions Number of repetitions of the signal
     */
    void LEDSignal(uint16_t time_ms, uint16_t pause_ms, uint8_t repetitions);

    /**
     * @brief Triggers the LED and Buzzer Signal; is Blocking!
     * @param color Color of the LED signal (0xRRGGBBWW)
     * @param time_ms Duration of the signal in milliseconds
     * @param pause_ms Pause between signals in milliseconds
     * @param repetitions Number of repetitions of the signal
     */
    void LED_BUZZER_Signal(uint16_t time_ms, uint16_t pause_ms, uint8_t repetitions);

    /**
     * @brief Set the LED Illumination of the Robot (Light for Camera)
     * @param color Color of the Illumination (0xRRGGBBWW)
     */
    void SetIllumination(uint32_t color);

    /**
     * @brief Gets the Battery Charge Level in Percent
     * @return Charge Level (0-100)
     */
    uint8_t GetCharge();
};