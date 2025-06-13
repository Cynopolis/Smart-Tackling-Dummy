/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once
#include "SerialMessage.h"
#include "BluetoothSerial.h"

class BluetoothSerialMessage : public SerialMessage{
    public:
        /**
         * @brief Construct a new Bluetooth Serial Message object
         */
        BluetoothSerialMessage(BluetoothSerial *serial);

        /**
         * @brief Initialize the BluetoothSerialMessage object
         */
        void init(unsigned int baud_rate = 115200) override;

        /**
         * @brief prints the args array to the serial monitor
        */
        void printArgs() override;
    
    private:
        /**
         * @brief reads the serial data and stores it in the data array
        */
        void readSerial() override;

        BluetoothSerial *serial;


};