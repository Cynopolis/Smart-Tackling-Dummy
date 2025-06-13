/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once
#include "DataStream.h"
#include <Arduino.h>

struct xyzData{
    double x;
    double y;
    double z;

    double magnitude(){
        return sqrt(x*x + y*y + z*z);
    }

    
    void operator=(const xyzData& other){
        x = other.x;
        y = other.y;
        z = other.z;
    }
};
class sensorTemplate{
    public:
        sensorTemplate() = default;
        ~sensorTemplate() = default;

        /**
         * @brief initialize the imu
         * @returns None.
         */
        virtual bool init() = 0;

        /**
         * @brief run any calibration necessary
         * @returns None.
         */
        virtual void calibrate() = 0;

        /**
         * @brief do all necessary updates to the device
         * @param data an array of data to perform the update with
         * @returns None.
         */
        virtual void update(double* data);

        /**
         * @brief get the last data read from the device
         * @returns the last data read from the device
         */
        virtual xyzData* getData();

        /**
         * @brief reset the peak data to 0
         * @returns None.
         */
        virtual void resetPeaks();

        /**
         * @brief get the peak data
         * @returns the peak data as a 6 element long array for acceleration and gyro
         */
        virtual xyzData* getPeaks();

        /**
         * @brief get the data stream for this device
         * @returns a pointer to the data stream for this device
         */
        DataStream<xyzData>* getDataStream();

        /**
         * @brief set the header for the data stream
         * @param header the header to set
         * @param length the length of the header
        */
        virtual void setHeader(char * header, unsigned int length);

    protected:
        xyzData data = {0,0,0};
        xyzData offset = {0,0,0};
        xyzData peak_data = {0,0,0};
        double peak_mag = 0;
        bool initialized = false; // true if the IMU has been initialized

        unsigned long lastUpdateTime = 0;

        DataStream<xyzData> stream;
};