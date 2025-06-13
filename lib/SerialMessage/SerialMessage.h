/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once

#include "Arduino.h"

#define num_chars 50
// define some constants
#define IMU_READ 0
// when you recieve a message with the first arg of 0, you know it's an IMU_READ message
#define IMU_WRITE 1
#define HIGH_G_ACCEL_READ 2
#define HIGH_G_ACCEL_WRITE 3
#define TENSION_READ 4
#define TENSION_WRITE 5
#define IMPACT_READ 6
#define IMPACT_WRITE 7

class SerialMessage{
    public:
        /**
         * @brief Construct a new Serial Message object
         */
        SerialMessage(HardwareSerial *serial = &Serial);

        /**
         * @brief Initialize the SerialMessage object
         */
        virtual void init(unsigned int baud_rate = 115200);

        /**
         * @brief Update the SerialMessage object and parse any data that's available
         */
        void update();

        /**
         * @brief Returns true if there is new data available
         * @return true if there is new data available
         */
        bool isNewData();

        /**
         * @brief Clears the new data flag
         */
        void clearNewData();

        /**
         * @brief Return a pointer to the args array
         * @return a pointer to the args array
         */
        int * getArgs();

        /**
         * @brief Returns the number of args that have been populated for the current message
         * @return the number of args that have been populated for the current message
         */
        int getArgsLength();

        /**
         * @brief Returns the number of args that have been populated for the current message
         * @return the number of args that have been populated for the current message
         */
        int getPopulatedArgs();

        /**
         * @brief Prints the args array to the serial monitor
         */
        virtual void printArgs();

    protected:
        virtual void readSerial();
        void parseData();

        bool new_data = false;
        bool data_recieved = false;
        bool recvInProgress = false;
        char data[num_chars]; // an array to store the received data
        char temp_data[num_chars]; // an array that will be used with strtok()
        uint8_t ndx = 0;
        const static int args_length = 30;
        int populated_args = 0; // the number of args that have been populated for the current message
        int args[args_length];
        const char startMarker = '!';
        const char endMarker = ';';
    
    private:
        HardwareSerial *serial;
};