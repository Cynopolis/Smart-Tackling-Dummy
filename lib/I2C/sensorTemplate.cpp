/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "sensorTemplate.h"
#include <Arduino.h>

void sensorTemplate::update(double* data){
    // use the xyzData struct to store the data
    this->data = {data[0], data[1], data[2]};
    this->stream.prepend(this->data);
    double mag = this->data.magnitude();
    if(mag > this->peak_mag){
        this->peak_mag = mag;
        this->peak_data = this->data;
    }
}

xyzData* sensorTemplate::getData(){
    return &(this->data);
}

xyzData* sensorTemplate::getPeaks(){
    return &(this->peak_data);
}

void sensorTemplate::resetPeaks(){
    this->peak_mag = 0;
    this->peak_data = {0, 0, 0};
}

DataStream<xyzData>* sensorTemplate::getDataStream(){
    return &(this->stream);
}

void sensorTemplate::setHeader(char * header, unsigned int length){
    this->stream.setHeader(header, length);
}