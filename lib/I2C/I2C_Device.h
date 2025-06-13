/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once
#include <Arduino.h>
#include <Wire.h>

// this is a pure virtual class which is used to define the interface for all I2C devices
// all other I2c devices will extend this interface
class I2C_Device{
    public:

        I2C_Device(TwoWire *i2cBus, uint8_t address, uint8_t dataLength)
        : address(address), dataLength(dataLength){
            this->i2cBus = i2cBus;
        }
        /**
         * @brief Do any action necessary to initialize the device
         * @returns None.
         */
        virtual bool init() = 0;

        /**
         * @bief write data to the device
         * @param data ann array of data to write to the device
         * @param length the length of the data array
         * @returns None.
         */
        virtual void write(uint8_t data[], uint8_t length) = 0;

        /**
         * @brief do all necessary updates to the device
         * @returns None.
         */
        virtual void update() = 0;

        /**
         * @brief get the last data read from the device
         * @returns the last data read from the device
         */
        virtual double* getData() = 0;

        /**
         * @brief Get the data array length
         * @returns the length of the data array
         */
        virtual uint8_t getDataLength(){return dataLength;};


    protected:
        // the address of the I2C device
        uint8_t address;
        // The i2c bus object to be used by this sensor
        TwoWire* i2cBus;
        // the length of the data array
        uint8_t dataLength;

        /**
         * @brief calculate the magnitude of a 3d vector
         * @param data a 3 element array of doubles
         * @returns the magnitude of the vector
         */
        double magnitude(double* data){
            return sqrt(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
        };
};