/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once

#include "I2C_Device.h"
#include "sensorTemplate.h"
#include <Adafruit_H3LIS331.h>

class accelSensor : public sensorTemplate{
    public:
        /**
         * @brief Do any action necessary to initialize the device
         * @param wire A pointer to the I2C bus to use
         * @param address The I2C address of the device
         * @returns None.
         */
        bool init(TwoWire * wire, uint8_t address);

        /**
         * @brief Initialize the device with a deffault device address and wire
         * @returns None.
         */
        bool init() override;

        /**
         * @brief do all necessary updates to the device
         * @returns None.
         */
        void update();

        /**
         * @brief do all of the necessary updates to the device while taking the accelerometer's angle into account.
         * @param angle xyzData The angle of the accelerometer
        */
        void update(const xyzData angle);

        /**
         * @brief run any calibration necessary
         * @returns None.
         */
        void calibrate() override;

        /**
         * @brief Set the header for the data stream
         * @param header A pointer to the header array
         * @param length The length of the header array
        */
        void setHeader(char* header, unsigned int length) override;

    private:
        // processes and stores data from the IMU
        Adafruit_H3LIS331 accel;
        // the location of this accelerometer
        char location[4];
        // the length of the data returned by the sensor
        uint8_t dataLength;
        xyzData calibData = {0, 0, 0};
};