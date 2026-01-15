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
#include <vl53l4cd_class.h>
#include <vl53l4cd_api.h>
#include "CustomDatatypes.h"

#ifdef _MSC_VER
    #pragma endregion
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
    uint8_t xshutPin;           // to store the XSHUT Pin of each sensor
    uint8_t i2cAddress;         // address of each sensor
    uint16_t lastMeasurement;   // value of the last range measurement
    TofStatus lastStatus;       // status of the last measurement
    uint32_t measurementCount;  // number of measurements since start
    uint32_t ts_lastMeasurement;// timestamp of the last measurement; to detect a TIMEOUT
    bool newData;               // bool to store if a measurement is new or has been read used already
};

#ifdef _MSC_VER
    #pragma endregion
    #pragma region VL6180X //----------------------------------------------------------------------------------------------------
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
    TofStatus GetStatus(void);


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
    TofStatus GetStatus(void);

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
    #pragma region TOF Class //--------------------------------------------------------------------------------------------------
#endif
class TofSensors {
    private:
    // I2C addresses for the sensors
    #define I2C_ADDRESS_SLF 0x23
    #define I2C_ADDRESS_SLB 0x23
    #define I2C_ADDRESS_SRF 0x23
    #define I2C_ADDRESS_SRB 0x23
    #define I2C_ADDRESS_MF  0x23
    #define I2C_ADDRESS_MB  0x23

    // XSHUT pins for the sensors
    #define XSHUT_PIN_SLF 0x23
    #define XSHUT_PIN_SLB 0x23
    #define XSHUT_PIN_SRF 0x23
    #define XSHUT_PIN_SRB 0x23
    #define XSHUT_PIN_MF  0x23
    #define XSHUT_PIN_MB  0x23

    // Ranging budgets for the sensors
    #define RANGING_BUDGET_SHORT 30
    #define RANGING_BUDGET_MID 20

    public:
    // Objects
    TofVL180X shortLeftFront = TofVL180X(I2C_ADDRESS_SLF, XSHUT_PIN_SLF);
    TofVL180X shortRightFront = TofVL180X(I2C_ADDRESS_SRF, XSHUT_PIN_SRF);
    TofVL180X shortLeftBack = TofVL180X(I2C_ADDRESS_SLB, XSHUT_PIN_SLB);
    TofVL180X shortRightBack = TofVL180X(I2C_ADDRESS_SRB, XSHUT_PIN_SRB);
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
};
#ifdef _MSC_VER
    #pragma endregion
#endif