/**
 * @file IRCom.cpp
 * @brief IR remote control class
 * @author Quinn Henthorne. Email: henth013@d.umn.edu, Phone: 763-656-8391
 * @version 1.0
 * @date 03-29-23
*/

#pragma once

#include <IRremoteESP8266.h>
#include <IRrecv.h>

typedef enum{
    nothing,
    toggleRecord,
    leftLight,
    rightLight,
    reset
} IRCode;

class IRCom {
    public:
        /**
         * @brief Construct a new IRCom object
         * @param pin 
         */
        IRCom(uint8_t pin);

        /**
         * @brief Initialize the IRCom object
         */
        void init();

        /**
         * @brief Update the IRCom object
         */
        void update();

        /**
         * @brief Get the oldest IRCode from the queue
         * @post The oldest IRCode will be removed from the queue
         * @return IRCode 
         */
        IRCode popCode();

        /**
         * @brief Returns the number of IRCodes in the queue
         * @return uint8_t the number of IRCodes in the queue. (up to 3)
         */
        uint8_t codesAvailable();

    private:
        uint8_t codesAvailableCount = 0;
        static const uint8_t maxCodes = 3;
        uint8_t pin;
        IRCode results[maxCodes] = {nothing, nothing, nothing};
        IRrecv * receiver;
        /**
         * @brief Append an IRCode to the queue
         */
        void pushCode(IRCode code);

};