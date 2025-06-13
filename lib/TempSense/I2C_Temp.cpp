/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "I2C_Temp.h"
#include <Wire.h>

I2C_Temp::I2C_Temp(TwoWire *i2cBus, uint8_t address) : I2C_Device(i2cBus, address, 1){}

bool I2C_Temp::init(){
    for(int i = 0; i < 5; i++){
        Serial.println("Initializing temperature sensor at address: 0x" + String(address, HEX) + "...");
        if(this->temp_sensor.begin(i2cBus, address)){
            this->initialized = true;
            Serial.println("Temperature sensor initialized at address: 0x" + String(address, HEX));
            this->initialized = true;
            unsigned long timer = millis();
            return true;
        }
        Serial.println("Couldn't find temperature sensor at address: " + String(address, HEX) + ".\n Attempt (" + String(i+1) + "/5) Trying again in 1 second...");
    }
    Serial.println("Couldn't find temperature sensor at address: " + String(address, HEX) + ". Check the wiring and try again.");
    return false;
}

// do nothing here
void I2C_Temp::write(uint8_t data[], uint8_t length){}

void I2C_Temp::update(){
    if(!initialized){
        Serial.println("Temperature sensor not initialized. Call init() before update().");
        return;
    }
    sensors_event_t humidity, temp;
    // update the temperature every 1 second
    if(millis() - lastTempTime > 1000){
        temp_sensor.getEvent(&humidity, &temp);

        this->temp = temp.temperature;
        // humdity is: humidity.relative_humidity (in rH)

        lastTempTime = millis();
    }
}

double* I2C_Temp::getData(){
    returnData[0] = temp;
    return returnData;
}

