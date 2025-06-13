/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once
#include "I2C_Device.h"
#include <Adafruit_AHTX0.h>
#include <Wire.h>

class I2C_Temp : public I2C_Device{
    public:
        /**
         * @brief Construct a new I2C_Temp object
         * @param i2cBus the i2c bus to use
         * @param address the address of the I2C_Temp
         * @param location the location of the I2C_Temp (can be four characters long)
         */
        I2C_Temp(TwoWire *i2cBus, uint8_t address);

        /**
         * @brief Do any action necessary to initialize the device
         * @returns None.
         */
        bool init() override;
        /**
         * @brief write data to the device so it reads at maximum range and speeds. Does not use the data array
         * @param data ann array of data to write to the device
         * @param length the length of the data array
         * @returns None.
         */
        void write(uint8_t data[], uint8_t length) override;

        /**
         * @brief do all necessary updates to the device
         * @returns None.
         */
        void update() override;

        /**
         * @brief get the last data read from the device
         * @returns the last data read from the device
         */
        double* getData() override;

    private:
        // the last temp data read from the device in celcius
        float temp = 0;

        // in milliseconds
        unsigned long lastTempTime;

        double returnData[1] = {0};

        bool initialized = false; // whether or not the device has been initialized

        // the location of the device
        char location[4];

        // the temperature sensor
        Adafruit_AHTX0 temp_sensor;



};