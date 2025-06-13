/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once
#include <Arduino.h>

class StatusLight{
    public:
        /**
         * @brief Construct a new Satus Light object
         * @param led_num the position of the led on the LED strip
         */
        StatusLight(uint8_t led_num);

        /**
         * @brief Set the color of the LED when the status is low
         * @param low_color 
         */
        void set_low_color(uint8_t * low_color);

        /**
         * @brief Set the color of the LED when the status is high
         * @param high_color 
         */
        void set_high_color(uint8_t * high_color);

        /**
         * @brief Update the LED color based on a percentage form 0 to 100
         * @param percent 
         */
        virtual void update(){return;};

        /**
         * @brief Get the current color of the LED
         * @return a pointer to an array of 3 uint8_t's
         */
        uint8_t * get_current_color();

        virtual void print(){return;};

    protected:
        // the color of the LED when the status is low
        uint8_t low_color[3] = {0,127,0};
        // the color of the LED when the status is high
        uint8_t high_color[3] = {127,0,0};
        // the current color of the LED
        uint8_t current_color[3] = {0,127,0};
        // the position of the led on the LED strip
        uint8_t led_num;

        /**
         * @brief copies the values of one array to another
         * @param input_array the array to copy from
         * @param output_array the array to copy to
        */
        void copy_array(uint8_t * input_array, uint8_t * output_array);
};