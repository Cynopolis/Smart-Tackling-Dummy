/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "imuAccel.h"

imuAccel::imuAccel(Adafruit_LSM6DSOX* imu){
    this->imu = imu;
}

bool imuAccel::init(){
    // set the accelerometer range and then confirm that it was succesffully set
    this->imu->setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
    Serial.print("Accelerometer range set to: ");
    switch (this->imu->getAccelRange()) {
    case LSM6DS_ACCEL_RANGE_2_G:
        Serial.println("+-2G");
        break;
    case LSM6DS_ACCEL_RANGE_4_G:
        Serial.println("+-4G");
        break;
    case LSM6DS_ACCEL_RANGE_8_G:
        Serial.println("+-8G");
        break;
    case LSM6DS_ACCEL_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }

    // set the accelerometer data rate and then confirm that it was succesffully set
    this->imu->setAccelDataRate(LSM6DS_RATE_6_66K_HZ);
    Serial.print("Accelerometer data rate set to: ");
    switch (this->imu->getAccelDataRate()) {
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
    this->getDataStream()->setInitialized(true);
    return true;
}

void imuAccel::calibrate(){
    // take 1000 readings and average them to determine the offset
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    
    for(int i = 0; i < 1000; i++){
        this->imu->getEvent(&accel, &gyro, &temp);
        this->offset.x += accel.acceleration.x;
        this->offset.y += accel.acceleration.y;
        this->offset.z += accel.acceleration.z;
        delay(1);
    }

    this->offset.x /= (1000 * 9.80665); // convert to Gs
    this->offset.y /= (1000 * 9.80665); // convert to Gs
    this->offset.z /= (1000 * 9.80665); // convert to Gs
}

void imuAccel::update(const xyzData &gyroAngle, sensors_event_t &data){
    double data_array[3] = {
        ((double)data.acceleration.x) / 9.80665 - offset.x, 
        ((double)data.acceleration.y) / 9.80665 - offset.y,
        ((double)data.acceleration.z) / 9.80665 - offset.z
    };

    // explicitly call the update function in the parent class
    // the data stream will now keep track of a gravity-less vector
    sensorTemplate::update(data_array);
}

void imuAccel::setHeader(char * header, unsigned int length){
    char accel[] = "LowGAccel";
    // create a new header that is the old header + the gyro header
    char* newHeader = new char[length + sizeof(accel)];
    // copy the old header and the gyro header into the new header
    memcpy(newHeader, header, length);
    memcpy(newHeader + length, accel, sizeof(accel));
    // call the parent class's setHeader function
    sensorTemplate::setHeader(newHeader, length + sizeof(accel));
}
