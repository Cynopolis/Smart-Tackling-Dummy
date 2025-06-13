/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "I2C_Accel.h"

I2C_Accel::I2C_Accel(TwoWire *i2cBus, uint8_t address, char* location) : I2C_Device(i2cBus, address, 3){
    this->location[0] = location[0];
    this->location[1] = location[1];
    this->location[2] = location[2];
    this->location[3] = location[3];

    this->accel.setHeader(location, 4);
}

bool I2C_Accel::init(){
    bool is_init = this->accel.init(this->i2cBus, this->address);
    this->initialized = is_init;
    return is_init;
}

void I2C_Accel::update(){
    this->accel.update();
}

void I2C_Accel::update(const xyzData angle){
    this->accel.update(angle);
}

double* I2C_Accel::getData(){
    xyzData *data = this->accel.getData();
    this->returnData[0] = (*data).x;
    this->returnData[1] = (*data).y;
    this->returnData[2] = (*data).z;
    return this->returnData;
}

DataStream<xyzData>* I2C_Accel::getDataStream(){
    return this->accel.getDataStream();
}

void I2C_Accel::setHeader(char* header, unsigned int length){
    this->accel.setHeader(header, length);
}

double* I2C_Accel::getPeaks(){
    xyzData *data = this->accel.getPeaks();
    this->returnData[0] = (*data).x;
    this->returnData[1] = (*data).y;
    this->returnData[2] = (*data).z;
    return this->returnData;
}