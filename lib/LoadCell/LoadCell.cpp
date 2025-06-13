/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "LoadCell.h"

LoadCell::LoadCell(uint8_t dataPin, uint8_t clockPin) :
    clockPin(clockPin), dataPin(dataPin){}

bool LoadCell::init(){
    loadCell.begin(dataPin, clockPin);
    loadCell.set_gain(64);
    for(int i = 0; i < 5; i++){
        if(loadCell.is_ready()){
            Serial.println("Load cell on pin: " + String(dataPin) + " was initialized on attempt (" + String(i+1) + "/5)");
            initialized = true;
            this->getDataStream()->setInitialized(true);
            return true;
        }
        Serial.println("Initialization of load cell on pin: " + String(dataPin) + " failed. Attempt (" + String(i+1) + "/5)");
        delay(100);
    }
    Serial.println("Failed to initialize load cell on pin: " + String(dataPin) + ". Check wiring.");
    return false;
}

void LoadCell::calibrate(double *calibrationTemps, double *outputValues, double *weightValues){
    // generate a calibration curve
    // y = ax^2 + bx + c
    // where x is the load cell reading
    // and y is the actual weight
    double x1 = outputValues[0];
    double x2 = outputValues[1];
    double x3 = outputValues[2];
    double y1 = weightValues[0];
    double y2 = weightValues[1];
    double y3 = weightValues[2];
    a = (-x1*y2+x1*y3+x2*y1-x2*y3-x3*y1+x3*y2)/(-x1*x2*x2+x1*x3*x3-x2*x3*x3+x1*x1*x2-x1*x1*x3+x2*x2*x3);
    b = (x1*x1*y2-x1*x1*y3-x2*x2*y1+x2*x2*y3+x3*x3*y1-x3*x3*y2)/(-x1*x2*x2+x1*x3*x3-x2*x3*x3+x1*x1*x2-x1*x1*x3+x2*x2*x3);
    c = (-x1*x2*x2*y3+x1*x3*x3*y2-x2*x3*x3*y1+x1*x1*x2*y3-x1*x1*x3*y2+x2*x2*x3*y1)/(-x1*x2*x2+x1*x3*x3-x2*x3*x3+x1*x1*x2-x1*x1*x3+x2*x2*x3);
    

    x1 = outputValues[3];
    x2 = outputValues[4];
    x3 = outputValues[5];
    y1 = weightValues[3];
    y2 = weightValues[4];
    y3 = weightValues[5];

    double c2 = (-x1*x2*x2*y3+x1*x3*x3*y2-x2*x3*x3*y1+x1*x1*x2*y3-x1*x1*x3*y2+x2*x2*x3*y1)/(-x1*x2*x2+x1*x3*x3-x2*x3*x3+x1*x1*x2-x1*x1*x3+x2*x2*x3);

    // calculate the temperature factor which is an offset used to adjust the calibration curve
    if(c2 == c || calibrationTemps[1] == calibrationTemps[0]){
        tempFactor = 0;
    }
    else{
        tempFactor = (c2 - c) / (calibrationTemps[1] - calibrationTemps[0]);
    }
    calibrationTemp = calibrationTemps[0];

    Serial.print("A: ");
    Serial.print(a,9);
    Serial.print(" B: ");
    Serial.print(b,9);
    Serial.print(" C: ");
    Serial.print(c,9);
    Serial.print(" Temp Factor: ");
    Serial.println(tempFactor);
}

void LoadCell::update(){
    if(!initialized){
        Serial.println("Load cell on pin: " + String(dataPin) + "not initialized.");
        return;
    }
    double reading = this->getWeight();
    if(reading > 0){
        lastReading = reading;
        dataStream.prepend(lastReading);
        if(abs(reading) > abs(peakImpact)) peakImpact = reading;
    }
}

double LoadCell::getData(){
    return lastReading;
}

double LoadCell::getPeaks(){
    return peakImpact;
}

void LoadCell::resetPeaks(){
    peakImpact = 0;
}

double LoadCell::getWeight(){
    if(loadCell.wait_ready_timeout(10)){
        double reading = double(loadCell.read());
        return a * pow(reading, 2) + b * reading + c + (tempFactor * (currentTemp - calibrationTemp));
    }
    return -1;
}

DataStream<double> *LoadCell::getDataStream(){
    return &dataStream;
}

void LoadCell::setCurrentTemp(double temp){
    currentTemp = temp;
}

void LoadCell::setLocation(char * location, unsigned int length){
    dataStream.setHeader(location, length);
}

char * LoadCell::getLocation(){
    return dataStream.getHeader();
}

unsigned int LoadCell::getLocationLength(){
    return dataStream.getHeaderLength();
}
