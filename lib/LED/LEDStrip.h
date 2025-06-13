/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once
#include <Adafruit_NeoPixel.h>
#include "StatusLight.h"


class LEDStrip{
    public:
        /**
         * @brief Construct a new LED object
         * @param pin The pin the LED strip is connected to
         */
        LEDStrip(uint8_t pin, StatusLight* status_lights[], unsigned int status_lights_length);

        /**
         * @brief Initialize the LED strip
         * @pre All status lights should be added before calling this function
         */
        void init();

        /**
         * @brief Update the LED strip
         */
        void update();

        /**
         * @brief Print the status of all the status lights
         */
        void print();

    private:
        uint8_t led_strip_pin;
        Adafruit_NeoPixel* pixels = NULL;
        StatusLight ** status_lights;
        unsigned int status_light_count;
};