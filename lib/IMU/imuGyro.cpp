/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "imuGyro.h"

imuGyro::imuGyro(Adafruit_LSM6DSOX* imu){
    this->imu = imu;
}

bool imuGyro::init(){
    // set the gyro range and then confirm that it was succesffully set
    this->imu->setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
    Serial.print("Gyro range set to: ");
    switch (this->imu->getGyroRange()) {
        case LSM6DS_GYRO_RANGE_125_DPS:
            Serial.println("125 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_250_DPS:
            Serial.println("250 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_500_DPS:
            Serial.println("500 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_1000_DPS:
            Serial.println("1000 degrees/s");
            break;
        case LSM6DS_GYRO_RANGE_2000_DPS:
            Serial.println("2000 degrees/s");
            break;
        case ISM330DHCX_GYRO_RANGE_4000_DPS:
            break; // unsupported range for the DSOX
    }

    // set the gyro data rate and then confirm that it was succesffully set
    this->imu->setGyroDataRate(LSM6DS_RATE_6_66K_HZ);
    Serial.print("Gyro data rate set to: ");
    switch (this->imu->getGyroDataRate()) {
        case LSM6DS_RATE_SHUTDOWN:
            Serial.println("0 Hz");
            break;
        case LSM6DS_RATE_12_5_HZ:
            Serial.println("12.5 Hz");
            break;
        case LSM6DS_RATE_26_HZ:
            Serial.println("26 Hz");
            break;
        case LSM6DS_RATE_52_HZ:
            Serial.println("52 Hz");
            break;
        case LSM6DS_RATE_104_HZ:
            Serial.println("104 Hz");
            break;
        case LSM6DS_RATE_208_HZ:
            Serial.println("208 Hz");
            break;
        case LSM6DS_RATE_416_HZ:
            Serial.println("416 Hz");
            break;
        case LSM6DS_RATE_833_HZ:
            Serial.println("833 Hz");
            break;
        case LSM6DS_RATE_1_66K_HZ:
            Serial.println("1.66 KHz");
            break;
        case LSM6DS_RATE_3_33K_HZ:
            Serial.println("3.33 KHz");
            break;
        case LSM6DS_RATE_6_66K_HZ:
            Serial.println("6.66 KHz");
            break;
    }

    this->lastUpdateTime = micros();
    this->getDataStream()->setInitialized(true);
    return true;
}

void imuGyro::calibrate(){
    // take 1000 readings and average them to determine the offset
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    
    for(int i = 0; i < 1000; i++){
        this->imu->getEvent(&accel, &gyro, &temp);
        this->offset.x += gyro.gyro.x;
        this->offset.y += gyro.gyro.y;
        this->offset.z += gyro.gyro.z;
        delay(1);
    }

    this->offset.x /= 1000;
    this->offset.y /= 1000;
    this->offset.z /= 1000;
}

void imuGyro::update(sensors_event_t &data){
    double dt = double(micros() - this->lastUpdateTime) / 1000000;
    double offset_data[3] = {
        data.gyro.x - this->offset.x,
        data.gyro.y - this->offset.y,
        data.gyro.z - this->offset.z
    };

    // calculate the angular acceleration in rad/s^2
    double data_array[3] = {
        offset_data[0] * dt,
        offset_data[1] * dt,
        offset_data[2] * dt
    };
    // explicitly call the update function in the parent class
    // The data stream and data object will now keep track of angular acceleration
    sensorTemplate::update(data_array);

    // update the accumulated rotation
    this->rotation.x += offset_data[0]*dt;
    this->rotation.y += offset_data[1]*dt;
    this->rotation.z += offset_data[2]*dt;

    this->lastUpdateTime = micros();
}

xyzData* imuGyro::getRotation(){
    return &this->rotation;
}

void imuGyro::setHeader(char* header, unsigned int length){
    char gyro[] = "Gyro";
    // create a new header that is the old header + the gyro header
    char* newHeader = new char[length + sizeof(gyro)];
    // copy the old header and the gyro header into the new header
    memcpy(newHeader, header, length);
    memcpy(newHeader + length, gyro, sizeof(gyro));
    // call the parent class's setHeader function
    sensorTemplate::setHeader(newHeader, length + sizeof(gyro));
}