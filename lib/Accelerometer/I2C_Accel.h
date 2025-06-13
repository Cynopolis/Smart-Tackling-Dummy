/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once

#include "I2C_Device.h"
#include "accelSensor.h"
#include "DataStream.h"

class I2C_Accel : public I2C_Device{
    public:
        /**
         * @brief Construct a new I2C Accel object
         * @param address the address of the I2C device
         * @param location the location of the I2C device
         */
        I2C_Accel(TwoWire *i2cBus, uint8_t address, char* location);

        /**
         * @brief Do any action necessary to initialize the device
         * @returns None.
         */
        bool init() override;

        /**
         * @brief write data to the device. (Does nothing)
         * @param data an array of data to write to the device
         * @param length the length of the data array
         * @returns None.
         */
        void write(uint8_t data[], uint8_t length) override {};

        /**
         * @brief do all necessary updates to the device
         * @returns None.
         */
        void update() override;

        /**
         * @brief do all of the necceary updates to the device while taking the device's rotation into account
         * @param rotation the rotation of the device
        */
        void update(const xyzData angle);

        /**
         * @brief get the last data read from the device
         * @returns the last data read from the device
         */
        double* getData() override;

        /**
         * @brief gets a pointer to the DataStream object for this device
         * @returns a pointer to the DataStream object for this device
         */
        DataStream<xyzData>* getDataStream();

        /**
         * @brief Set the header for the data stream
         * @param header A pointer to the header array
         * @param length The length of the header array
         */
        void setHeader(char* header, unsigned int length);

        /**
         * @brief Get the peek data
         * @returns the peek data as a 3 element long array for acceleration
         */
        double* getPeaks();

        /**
         * @brief reset the peek data to 0
        */
        void resetPeaks(){accel.resetPeaks();};

        /**
         * @brief returns if the device is initialized
         * @returns true if the device is initialized
         */
        bool isInitialized(){return initialized;};

    private:
        accelSensor accel;
        char location[4];
        double returnData[3] = {0};
        bool initialized = false;


};