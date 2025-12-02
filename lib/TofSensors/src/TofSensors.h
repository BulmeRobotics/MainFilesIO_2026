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
#include <VL6180X.h>
#include <vl53l4cd_class.h>
#include <vl53l4cd_api.h>
#include "CustomDatatypes.h"

#ifdef _MSC_VER
    #pragma endregion
    #pragma region TOF Class //--------------------------------------------------------------------------------------------------
#endif
class TofSensors {
    private:
    // I2C addresses for the sensors
    #define I2C_ADDRESS_SFL 0x23
    #define I2C_ADDRESS_SFR 0x23
    #define I2C_ADDRESS_SBL 0x23
    #define I2C_ADDRESS_SBR 0x23
    #define I2C_ADDRESS_MF  0x23
    #define I2C_ADDRESS_MB  0x23

    // XSHUT pins for the sensors
    #define XSHUT_PIN_SFL 0x23
    #define XSHUT_PIN_SFR 0x23
    #define XSHUT_PIN_SBL 0x23
    #define XSHUT_PIN_SBR 0x23
    #define XSHUT_PIN_MF  0x23
    #define XSHUT_PIN_MB  0x23

    public:
    // Constructor
    TofSensors();

    // Objects
    TofVL180X shortFrontLeft = TofVL180X(I2C_ADDRESS_SFL, XSHUT_PIN_SFL);
    TofVL180X shortFrontRight = TofVL180X(I2C_ADDRESS_SFR, XSHUT_PIN_SFR);
    TofVL180X shortBackLeft = TofVL180X(I2C_ADDRESS_SBL, XSHUT_PIN_SBL);
    TofVL180X shortBackRight = TofVL180X(I2C_ADDRESS_SBR, XSHUT_PIN_SBR);
    TofVL53L4CD midFront = TofVL53L4CD(I2C_ADDRESS_MF, XSHUT_PIN_MF);
    TofVL53L4CD midBack = TofVL53L4CD(I2C_ADDRESS_MB, XSHUT_PIN_MB);

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
    *         TIMEOUT if a sensor is not reachable
    */
    ErrorCodes Update(void);

    /**
    * @brief  Getter-method to access to last range measurement of a Time-of-Flight-Sensors. 
    * @param  sensor enum to choose the sensor to get the value from.
    * @return Last range measurement in mm.
    */
    uint16_t GetRange(TofType sensor);
};

#ifdef _MSC_VER
    #pragma endregion
    #pragma region Parent Class //-----------------------------------------------------------------------------------------------
#endif
class TofParent {
    public:
    // Destructor
    virtual ~TofParent() = default;

    // Methods (to be overwritten by child classes)
    virtual bool Init(void);
    virtual ErrorCodes Stop(void);
    virtual ErrorCodes Read(void);

    private:
    // Variables
    uint8_t xshutPin;           // to store the XSHUT Pin of each sensor
    uint8_t i2cAddress;         // address of each sensor
    uint32_t measurementCount;  // number of measurements since start
    uint32_t lastMeasurement;   // timestamp of the last measurement; to detect a TIMEOUT
    bool newData;               // bool to store if a measurement is new or has been read used already
};

#ifdef _MSC_VER
    #pragma endregion
    #pragma region VL6180X //-----------------------------------------------------------------------------------------------
#endif
class TofVL180X : public TofParent {
    private:
    // Object
    VL6180X sensor;
    
    public:
    // Constructor
    TofVL180X(uint8_t i2cAddress , uint8_t xshutPin);
    
    // Methods
    /**
    * @brief  Initializes and configures the specific Time-of-Flight-Sensor.
    * @return OK if the sensors was intitalized succesfully.
    *         ERROR if something went wrong.
    */
	bool Init(void) override;

    /**
    * @brief  Stops the ranging of the Time-of-Flight-Sensor.
    * @return OK if the sensors was stopped succesfully.
    *         ERROR if something went wrong.
    */
	ErrorCodes Stop(void) override;

    /**
    * @brief  Checks if a new measurement is available; if so retrieve the data
    * @param  sensor enum to choose the sensor to get the value from.
    * @return OK if no new data is available.
    *         NEW_DATA if data was retrieved and stored
    */
    ErrorCodes Read(void) override;
};

#ifdef _MSC_VER
    #pragma endregion
    #pragma region VL53L4CD //-----------------------------------------------------------------------------------------------
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
    * @return OK if the sensors was intitalized succesfully.
    *         ERROR if something went wrong.
    */
	bool Init(void) override;

    /**
    * @brief  Stops the ranging of the Time-of-Flight-Sensor.
    * @return OK if the sensors was stopped succesfully.
    *         ERROR if something went wrong.
    */
	ErrorCodes Stop(void) override;
    
    /**
    * @brief  Checks if a new measurement is available; if so retrieve the data
    * @param  sensor enum to choose the sensor to get the value from.
    * @return OK if no new data is available.
    *         NEW_DATA if data was retrieved and stored
    */
    ErrorCodes Read(void) override;
};