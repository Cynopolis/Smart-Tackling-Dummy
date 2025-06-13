/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once

#include "sensorTemplate.h"
#include <Adafruit_LSM6DSOX.h>


class imuGyro : public sensorTemplate{
    public:
        imuGyro(){};

        imuGyro(Adafruit_LSM6DSOX* imu);

        bool init() override;

        /**
         * @brief Calibrate the IMU
         */
        void calibrate() override;

        /**
         * @brief Update the IMU with new data
         * @param data 
         */
        void update(sensors_event_t &data);

        /**
         * @brief Get the Rotation object
         * @return xyzData* of accumulated rotation (current angle)
         */
        xyzData* getRotation();

        /**
         * @brief Set the header for the data stream
         * @param header 
         * @param length 
         */
        void setHeader(char* header, unsigned int length) override;
        
        Adafruit_LSM6DSOX* imu;

        xyzData rotation = {0, 0, 0};

        /**
         * @brief Calculate the current rotation of the IMU
         */
        void calculateRotation();
};