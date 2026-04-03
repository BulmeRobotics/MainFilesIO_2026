#pragma once
/**
* @name:    TofSensors.h
* @date:	18.11.2025
* @authors: Florian Wiesner
* @details: Header file for Time-of-Flight-Sensors 
*/

#ifdef _MSC_VER
    #pragma region Libraries //--------------------------------------------------------------------------------------------------
#endif
#include <Arduino.h>
#include <Wire.h>
#include "VL6180X_Custom/VL6180X_Custom.h"
#include "VL53L4CD_Custom/src/vl53l4cd_class.h"
#include "VL53L4CD_Custom/src/vl53l4cd_api.h"
#include <SparkFun_VL53L5CX_Library.h>
#include "CustomDatatypes.h"

#ifdef _MSC_VER
    #pragma region Parent Class //-----------------------------------------------------------------------------------------------
#endif
class TofParent {
    public:
    // Constructor
    TofParent(uint8_t i2cAddress, uint8_t xshutPin);

    // Destructor
    virtual ~TofParent() = default;

    // Methods (to be overwritten by child classes)
    virtual bool Init(void);
    virtual ErrorCodes Read(void);
    virtual uint16_t GetRange(void);
    virtual TofStatus GetStatus(void);
    virtual bool IsDataNew(void);
    virtual bool TimeoutOccured(void);
    virtual ErrorCodes Stop(void);
    virtual ErrorCodes Continue(void);


    // General methods for all child sensors
    /**
    * @brief  Disables the sensor by turning the XSHUT pin LOW
    */
    void Off(void);

    /**
    * @brief  Enables the sensor by releasing the XSHUT pin
    */
   void On(void);

    protected:
    // Variables (accessible in child classes)
    uint8_t xshutPin;               // to store the XSHUT Pin of each sensor
    uint8_t i2cAddress;             // address of each sensor
    uint16_t lastMeasurement;       // value of the last range measurement
    TofStatus lastStatus;           // status of the last measurement
    uint32_t measurementCount;      // number of measurements since start
    uint32_t ts_lastMeasurement;    // timestamp of the last measurement; to detect a TIMEOUT
    uint32_t ts_waitStart;          // start of the current wait for the data
    uint32_t ts_lastReadCall;       // last call of Read()
    bool timeoutOccured;
    bool waitingForMeasurement; 
    bool newData;                   // bool to store if a measurement is new or has been read used already

    static constexpr uint16_t TIMEOUT_TIME_MS = 500;
    static constexpr uint32_t INVALID_MEASUREMENT = 1023;
};


#ifdef _MSC_VER
    #pragma endregion
    #pragma region VL6180X //----------------------------------------------------------------------------------------------------
#endif
class TofVL6180X : public TofParent {
    private:
    // Object
    VL6180X sensor;
    
    public:
    // Constructor
    TofVL6180X(uint8_t i2cAddress , uint8_t xshutPin);
    
    // Methods
    /**
    * @brief  Initializes and configures the specific Time-of-Flight-Sensor.
    * @return true if the sensors was intitalized succesfully.
    */
	bool Init(void) override;

    /**
    * @brief  Checks if a new measurement is available; if so retrieve the data
    * @param  sensor enum to choose the sensor to get the value from.
    * @return OK if no new data is available.
    *         NEW_DATA if data was retrieved and stored.
    *         OUT_OF_RANGE if measurement was out of range.
    *         TIMEOUT if a timeout occured.
    */
    ErrorCodes Read(void) override;

    /**
    * @brief  Getter-method to get the value of the last measurement.
    * @return Last measurement value (0-255).
    *         ERROR if something went wrong.
    */
    uint16_t GetRange(void) override;

    /**
    * @brief  Getter-method to get the status of the last measurement.
    * @return VALID if the distance is not out of range or a timeout occured.
    *         OUT_OF_RANGE the measurement was out of range.
    *         TIMEOUT the sensor is in a timeout.
    */
    TofStatus GetStatus(void) override;

    /**
    * @brief  Method to check if a range value has yet been retrived. 
    * @return true if value is new.
    *         false if the value was already used.
    */
    bool IsDataNew(void) override;

    /**
    * @brief  Method to check if a Timeout has occured. 
    * @return true if a Timeout occured.
    *         false if no Timeout occured.
    */
    bool TimeoutOccured(void) override;


    /**
    * @brief  Stops the ranging of the Time-of-Flight-Sensor.
    * @return OK if the sensors was stopped succesfully.
    *         ERROR if something went wrong.
    */
	ErrorCodes Stop(void) override;

    /**
    * @brief  Continues the ranging of the Time-of-Flight-Sensor.
    * @return OK if the sensors is working correctly.
    *         ERROR if something went wrong.
    */
    ErrorCodes Continue(void) override;
};


#ifdef _MSC_VER
    #pragma endregion
    #pragma region VL53L4CD //---------------------------------------------------------------------------------------------------
#endif
class TofVL53L4CD : public TofParent {
    private:
    // Object
    VL53L4CD sensor;
    
    public:
    // Constructor
    TofVL53L4CD(uint8_t i2cAddress , uint8_t xshutPin);
    
    // Methods
    /**
    * @brief  Initializes and configures the specific Time-of-Flight-Sensor.
    * @return true if the sensors was intitalized succesfully.
    */
	bool Init(void) override;

    /**
    * @brief  Checks if a new measurement is available; if so retrieve the data
    * @param  sensor enum to choose the sensor to get the value from.
    * @return OK if no new data is available.
    *         NEW_DATA if data was retrieved and stored.
    *         OUT_OF_RANGE if measurement was out of range.
    *         TIMEOUT if a timeout occured.
    */
    ErrorCodes Read(void) override;

    /**
    * @brief  Getter-method to get the value of the last measurement.
    * @return Last measurement value (0-1023).
    *         ERROR if something went wrong.
    */
    uint16_t GetRange(void) override;

    /**
    * @brief  Getter-method to get the status of the last measurement.
    * @return VALID if the distance is not out of range or a timeout occured.
    *         OUT_OF_RANGE the measurement was out of range.
    *         TIMEOUT the sensor is in a timeout.
    */
    TofStatus GetStatus(void) override;

    /**
    * @brief  Method to check if a range value has yet been retrived. 
    * @return true if value is new.
    *         false if the value was already used.
    */
    bool IsDataNew(void) override;

    /**
    * @brief  Method to check if a Timeout has occured. 
    * @return true if a Timeout occured.
    *         false if no Timeout occured.
    */
    bool TimeoutOccured(void) override;

    /**
    * @brief  Stops the ranging of the Time-of-Flight-Sensor.
    * @return OK if the sensors was stopped succesfully.
    *         ERROR if something went wrong.
    */
	ErrorCodes Stop(void) override;

    /**
    * @brief  Continues the ranging of the Time-of-Flight-Sensor.
    * @return OK if the sensors is working correctly.
    *         ERROR if something went wrong.
    */
    ErrorCodes Continue(void) override;
};

#ifdef _MSC_VER
    #pragma endregion
    #pragma region VL53L5CX //---------------------------------------------------------------------------------------------------
#endif

class TofVL53L5CX : public TofParent{
    private:
		//Object
        SparkFun_VL53L5CX sensor;

        #define FRONT_MAX_DETECTION_DISTANCE 450
        #define FRONT_DIFF_DETECTION_VALUE 25

		VL53L5CX_ResultsData measurementData; 
		uint16_t imageResolution = 64;
		uint16_t imageWidth = sqrt(imageResolution);
		uint8_t messurePointsUpper[2] = {35 , 27};
		uint8_t messurePointsLower[2] = {36 , 28};

	public:	
        // Constructor
        TofVL53L5CX(uint8_t i2cAddress , uint8_t xshutPin);

        // Methods
        /**
         * @brief  Initializes and configures the specific Time-of-Flight-Sensor.
         * @return true if the sensors was intitalized succesfully.
         */
		bool Init(void) override;

        /**
         * @brief  Checks if a ramp is infront/behind the robot.
         * @return true if a ramp was detected.
         */
		bool IsRamp(void);
};


#ifdef _MSC_VER
    #pragma endregion
    #pragma region TOF Class //--------------------------------------------------------------------------------------------------
#endif
class TofSensors {
    private:

    // I2C addresses for the sensors
    #define I2C_ADDRESS_MLF 0x64
    #define I2C_ADDRESS_MLB 0x68
    #define I2C_ADDRESS_MRF 0x6C
    #define I2C_ADDRESS_MRB 0x70
    #define I2C_ADDRESS_MFU  0x74
    #define I2C_ADDRESS_MBU  0x78
    #define I2C_ADDRESS_MFL  0x7C
    #define I2C_ADDRESS_MBL  0x80
    #define I2C_ADDRESS_Fx64 0x46
    #define I2C_ADDRESS_Bx64 0x47

    // XSHUT pins for the sensors
    #define XSHUT_PIN_MLF A2
    #define XSHUT_PIN_MLB A5
    #define XSHUT_PIN_MRF A3
    #define XSHUT_PIN_MRB A4
    #define XSHUT_PIN_MFU  A7
    #define XSHUT_PIN_MBU  A6
    #define XSHUT_PIN_Fx64 32
    #define XSHUT_PIN_Bx64 26

    // Ranging budgets for the sensors
    #define RANGING_BUDGET_SHORT 30
    #define RANGING_BUDGET_MID 10

    // Objects
    TofVL53L4CD leftBack = TofVL53L4CD(I2C_ADDRESS_MLB, XSHUT_PIN_MLB);
    TofVL53L4CD leftFront = TofVL53L4CD(I2C_ADDRESS_MLF, XSHUT_PIN_MLF);
    TofVL53L4CD rightFront = TofVL53L4CD(I2C_ADDRESS_MRF, XSHUT_PIN_MRF);
    TofVL53L4CD rightBack = TofVL53L4CD(I2C_ADDRESS_MRB, XSHUT_PIN_MRB);
    TofVL53L4CD front = TofVL53L4CD(I2C_ADDRESS_MFU, XSHUT_PIN_MFU);
    TofVL53L4CD back = TofVL53L4CD(I2C_ADDRESS_MBU, XSHUT_PIN_MBU);
    TofVL53L5CX front_x64 = TofVL53L5CX(I2C_ADDRESS_Fx64, XSHUT_PIN_Fx64);
    TofVL53L5CX back_x64 = TofVL53L5CX(I2C_ADDRESS_Bx64, XSHUT_PIN_Bx64);


    // Member
    bool updateEnabled = true;

    // Method
    void DisableAll(void);

    public:
    // Constructor
    TofSensors() = default;

    // Methods
    /**
    * @brief  Initializes and configures all Time-of-Flight-Sensors.
    * @return OK if all sensors were intitalized succesfully.
    *         ERROR if something went wrong.
    */
    ErrorCodes Init(void);

    /**
    * @brief  Updates all Time-of-Flight-Sensors. If a new measurement is ready, it is stored in the corresponding sensor object.
    * @return OK if no data was updated.
    *         NEW_DATA if there were new values.
    *         NO_NEW_DATA if no new values were ready.
    *         TIMEOUT if a sensor is not reachable anymore.
    */
    ErrorCodes Update(void);

    /**
    * @brief  Methode to block the Update-Method.
    */
    void DisableUpdate(void);

    /**
    * @brief  Methode to re-enable the Update-Method.
    */
    void EnableUpdate(void);

    /**
    * @brief  Getter-method to access to last range measurement of a Time-of-Flight-Sensor. 
    * @param  sensor enum to choose the sensor to get the value from.
    * @return Last range measurement in mm.
    */
    uint16_t GetRange(TofType sensor);

    /**
    * @brief  Getter-method to access to last range measurement of a Time-of-Flight-Sensor. 
    * @param  sensor enum to choose the sensor to get the value from.
    * @return VALID if the distance is not out of range or a timeout occured.
    *         OUT_OF_RANGE the measurement was out of range.
    *         TIMEOUT the sensor is in a timeout.
    */
    TofStatus GetStatus(TofType sensor);

    /**
    * @brief  Method to check if a range value has yet been retrived. 
    * @param  sensor enum to choose the sensor to get the value from.
    * @return true if value is new.
    *         false if the value was already used.
    */
    bool IsDataNew(TofType sensor);

    /**
    * @brief  Method to check if a Timeout occured. 
    * @return true if a Timeout occured is new.
    *         false if no Timeout occured.
    */
    bool AnyTimeoutOccured(void);

    int8_t CalculateLeftRightError(float angleError, uint8_t sideWallThreshold, uint8_t gapRobotWall);
    uint8_t GetWalls(bool rampInfront, bool rampBehind);
};
#ifdef _MSC_VER
    #pragma endregion
#endif