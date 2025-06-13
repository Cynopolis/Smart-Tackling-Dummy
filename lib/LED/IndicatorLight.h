/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once
#include <Arduino.h>
#include "StatusLight.h"

class IndicatorLight : public StatusLight{
    public:
        /**
         * @brief Construct a new Satus Light object
         * @param led_num the position of the led on the LED strip
         * @param value a pointer to the value of the status
         * @param min_val the minimum value of the status
         * @param max_val the maximum value of the status
         * @param is_absolute if true, we only care about the absolute value of the status
         * @param is_inverted if true, we invert the status so a high value is low and a low value is high
         */
        IndicatorLight(uint8_t led_num, double (*value)(void), double min_val, double max_val, bool is_absolute, bool is_inverted);


        /**
         * @brief Update the LED color based on a percentage form 0 to 100
         * @param percent 
         */
        void update() override;

    protected:

        // a pointer to a function that returns the value of the status
        double (*value)(void);
        double min_val = 0;
        double max_val = 1;
        bool is_absolute = false;
        bool is_inverted = false;

        /**
         * @brief Interpolates between two colors by a given percent
         * @param percent from 0 to 100
         * @param low_color A pointer to an array of 3 uint8_t's
         * @param high_color A pointer to an array of 3 uint8_t's
         * @param output_array A pointer to an array of 3 uint8_t's
         * @return a color that is a percentage between the low and high colors
         */
        uint8_t * interpolate(uint8_t percent, uint8_t * low_color, uint8_t * high_color, uint8_t * output_array);

        /**
         * @brief Get the percent of the status
         * @return a percent from 0 to 100
         */
        uint8_t get_percent();

        void print() override;
};