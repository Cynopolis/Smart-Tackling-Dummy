/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once

#include "sensorTemplate.h"
#include <Adafruit_LSM6DSOX.h>


class imuAccel : public sensorTemplate{
    public:
        imuAccel(){};

        imuAccel(Adafruit_LSM6DSOX* imu);

        bool init() override;

        void calibrate() override;

        void update(const xyzData &gyroAngle, sensors_event_t &data);

        void setHeader(char * header, unsigned int length) override;

    private:
        Adafruit_LSM6DSOX* imu;

        xyzData noGravData;
};