/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "I2C_IMU.h"

I2C_IMU::I2C_IMU(TwoWire *i2cBus, uint8_t address, char* location) : I2C_Device(i2cBus, address, 6){
    // perform a manual deep copy of the location array
    this->location[0] = location[0];
    this->location[1] = location[1];
    this->location[2] = location[2];
    this->location[3] = location[3];

    this->gyro = imuGyro(&this->sox_IMU);
    this->accel = imuAccel(&this->sox_IMU);
    this->gyro.setHeader(location, 4);
    this->accel.setHeader(location, 4);
}

bool I2C_IMU::init(){
    for(int i = 0; i < 5; i++){
        if(this->sox_IMU.begin_I2C(this->address, this->i2cBus)){
            Serial.println("IMU succesfully initialized at address: 0x" + String(address, HEX));
            this->initialized = true;
            this->write(NULL, 0);
            // run a calibration setup
            this->calibrate();
            return true;
        }
        Serial.println("Failed to initialize IMU at location:" + String(this->location) + ". Attempt (" + String(i+1) + "/5)\n Attempting again in 1 second...");
        delay(50);
    }
    Serial.println("Failed to initialize IMU at location: " + String(this->location) + ". Please check your connections and try again.");
    return false;
}

void I2C_IMU::write(uint8_t data[], uint8_t length){
    if(!this->initialized){
        Serial.println("IMU not initialized. Please initialize the IMU before writing.");
        return;
    }
    this->gyro.init();
    this->accel.init();
}

void I2C_IMU::update(){
    if(!this->initialized){
        Serial.println("IMU not initialized. Please initialize the IMU before updating.");
        return;
    }
    // get the sensor data
    sensors_event_t accel_event;
    sensors_event_t gyro_event;
    sensors_event_t temp_event;
    this->sox_IMU.getEvent(&accel_event, &gyro_event, &temp_event);

    // update the gyro and accel
    this->gyro.update(gyro_event);
    this->accel.update(*(gyro.getRotation()), accel_event);
}

void I2C_IMU::calibrate(){
    if(!this->initialized){
        Serial.println("IMU not initialized. Please initialize the IMU before calibrating.");
        return;
    }
    // calibrate the gyro and accel
    this->gyro.calibrate();
    this->accel.calibrate();
}

double* I2C_IMU::getData(){
    if(!this->initialized){
        return this->returnData;
    }
    // get the data from the gyro and accel
    xyzData* gyroData = this->gyro.getData();
    xyzData* accelData = this->accel.getData();

    // copy the data into the returnData array
    this->returnData[0] = (*gyroData).x;
    this->returnData[1] = (*gyroData).y;
    this->returnData[2] = (*gyroData).z;
    this->returnData[3] = (*accelData).x;
    this->returnData[4] = (*accelData).y;
    this->returnData[5] = (*accelData).z;

    return this->returnData;
}

void I2C_IMU::resetPeaks(){
    // wait until the data is not being read before resetting the peaks
    this->gyro.resetPeaks();
    this->accel.resetPeaks();
}

double* I2C_IMU::getPeaks(){
    if(!this->initialized){
        return this->returnData;
    }
    // get the data from the gyro and accel
    xyzData* gyroData = this->gyro.getPeaks();
    xyzData* accelData = this->accel.getPeaks();

    // copy the data into the returnData array
    this->returnData[0] = (*gyroData).x;
    this->returnData[1] = (*gyroData).y;
    this->returnData[2] = (*gyroData).z;
    this->returnData[3] = (*accelData).x;
    this->returnData[4] = (*accelData).y;
    this->returnData[5] = (*accelData).z;

    return this->returnData;
}

double I2C_IMU::getAccelPeak(){
    xyzData accel = *(this->accel.getPeaks());
    return sqrt(pow(accel.x, 2) + pow(accel.y, 2) + pow(accel.z, 2));
}

double I2C_IMU::getGyroPeak(){
    xyzData gyro = *(this->gyro.getPeaks());
    return sqrt(pow(gyro.x, 2) + pow(gyro.y, 2) + pow(gyro.z, 2));
}

xyzData* I2C_IMU::getRotation(){
    return this->gyro.getRotation();
}

DataStream<xyzData>* I2C_IMU::getAccelStream(){
    return this->accel.getDataStream();
}

DataStream<xyzData>* I2C_IMU::getGyroStream(){
    return this->gyro.getDataStream();
}




