/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "StatusLight.h"

StatusLight::StatusLight(uint8_t led_num):
led_num(led_num){}

void StatusLight::set_low_color(uint8_t * low_color){
    this->copy_array(low_color, this->low_color);
}

void StatusLight::set_high_color(uint8_t * high_color){
    this->copy_array(high_color, this->high_color);
}

uint8_t * StatusLight::get_current_color(){
    return this->current_color;
}

void StatusLight::copy_array(uint8_t * input_array, uint8_t * output_array){
    output_array[0] = input_array[0];
    output_array[1] = input_array[1];
    output_array[2] = input_array[2];
}