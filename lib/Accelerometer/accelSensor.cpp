/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/


#include "accelSensor.h"

bool accelSensor::init(TwoWire * wire, uint8_t address) {
    for(int i = 0; i < 5; i++){
        // initialize the Accelerometer at 1000Hz, normal mode, and full scale of 400g
        if(this->accel.begin_I2C(address, wire)){
            Serial.println("Accelerometer succesfully initialized at address: 0x" + String(address, HEX));
            this->accel.setRange(H3LIS331_RANGE_400_G); // Set to maximum acceleration range
            this->accel.setDataRate(LIS331_DATARATE_1000_HZ); // Set to 1000Hz data rate
            this->initialized = true;
            this->calibrate();
            this->getDataStream()->setInitialized(true);
            return true;
        }
        Serial.println("Failed to initialize Accelerometer Attempt (" + String(i+1) + "/5).\n Attempting again in 1 second...");
        delay(50);
    }
    Serial.println("Failed to initialize Accelerometer at location: " + String(this->location) + "Please check your connections and try again.");
    return false;
}

bool accelSensor::init() {
    return this->init(&Wire, 0x19);
}

void accelSensor::update(){
    if(!this->initialized){
        Serial.println("Accelerometer not initialized. Please initialize the Accelerometer before updating.");
        return;
    }
    sensors_event_t event;
    this->accel.getEvent(&event);
    

    double data[3] = {event.acceleration.x - calibData.x, event.acceleration.y - calibData.y, event.acceleration.z - calibData.z};
    this->sensorTemplate::update(data);
}


void accelSensor::update(const xyzData angle){
    if(!this->initialized){
        Serial.println("Accelerometer not initialized. Please initialize the Accelerometer before updating.");
        return;
    }

    xyzData gravityVector = {
        sin(angle.x) * sin(angle.y) + cos(angle.x) * sin(angle.z) * cos(angle.y),
        cos(angle.x) * cos(angle.z) - sin(angle.x) * sin(angle.z) * cos(angle.y),
        cos(angle.x) * sin(angle.z) + sin(angle.x) * cos(angle.z) * cos(angle.y)
    };
    gravityVector = {gravityVector.x*this->offset.x, gravityVector.y*this->offset.y, gravityVector.z*this->offset.z};

    sensors_event_t event;
    this->accel.getEvent(&event);
    double data[3];
    data[0] = event.acceleration.x - gravityVector.x;
    data[1] = event.acceleration.y - gravityVector.y;
    data[2] = event.acceleration.z - gravityVector.z;
    this->sensorTemplate::update(data);
}


void accelSensor::calibrate() {
    if(!this->initialized){
        Serial.println("Accelerometer not initialized. Please initialize the Accelerometer before calibrating.");
        return;
    }
    double sum[3] = {0};
    sensors_event_t event;
    // take 1000 samples
    for (int i = 0; i < 1000; i++) {
        this->accel.getEvent(&event);
        sum[0] += event.acceleration.x;
        sum[1] += event.acceleration.y;
        sum[2] += event.acceleration.z;
        delay(1);
    }

    // calculate the average
    calibData.x = sum[0] / 1000;
    calibData.y = sum[1] / 1000;
    calibData.z = sum[2] / 1000;
}

void accelSensor::setHeader(char * header, unsigned int length){
    char accel[] = "HighGAccel";
    // create a new header that is the old header + the gyro header
    char* newHeader = new char[length + sizeof(accel)];
    // copy the old header and the gyro header into the new header
    memcpy(newHeader, header, length);
    memcpy(newHeader + length, accel, sizeof(accel));
    // call the parent class's setHeader function
    sensorTemplate::setHeader(newHeader, length + sizeof(accel));
}

