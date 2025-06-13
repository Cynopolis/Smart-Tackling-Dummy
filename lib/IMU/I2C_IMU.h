/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once

#include "I2C_Device.h"
#include <Adafruit_LSM6DSOX.h>
#include "imuGyro.h"
#include "imuAccel.h"


class I2C_IMU : public I2C_Device{
    public:
        /**
         * @brief Construct a new I2C_IMU object
         * @param wire a pointer to the i2c bus object to use
         * @param address the address of the I2C_IMU
         * @param location the location of the I2C_IMU (can be four characters long)
         */
        I2C_IMU(TwoWire *wire, uint8_t address, char* location);

        /**
         * @brief Do any action necessary to initialize the device
         * @returns None.
         */
        bool init() override;

        /**
         * @brief write data to the device so it reads at maxI2C_IMUm range and speeds. Does not use the data array
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

        /**
         * @brief Get the current rotation as measured by the gyro
         * @return xyzData* of accumulated rotation (current angle in radians
         */
        xyzData* getRotation();

        /**
         * @brief reset the peak data to 0
         * @returns None.
         */
        void resetPeaks();

        /**
         * @brief get the peak data
         * @returns the peak data as a 6 element long array for acceleration and gyro
         */
        double* getPeaks();

        /**
         * @brief get the peak magnitude of the acceleration
         * @returns the peak magnitude of the acceleration
         */
        double getAccelPeak();

        /**
         * @brief get the peak magnitude of the gyro
         * @returns the peak magnitude of the gyro
         */
        double getGyroPeak();

        /**
         * @brief get accelerometer datastream
         * @returns a pointer to the accelerometer datastream
         */
        DataStream<xyzData>* getAccelStream();

        /**
         * @brief get gyroscope datastream
         * @returns a pointer to the gyroscope datastream
         */
        DataStream<xyzData>* getGyroStream();

        /**
         * @brief returns true if properly initialized
        */
        bool isInitialized(){return initialized;};
    private:
        
        // holds data to be returned
        double returnData[6];
        // processes and stores data from the I2C_IMU
        Adafruit_LSM6DSOX sox_IMU;
        // location of the I2C_IMU
        char location[4];

        imuGyro gyro;
        imuAccel accel;
        bool initialized = false; // true if the device has been initialized

        /**
         * @brief run any calibration necessary for the device
         * @returns None.
         */
        void calibrate();
};