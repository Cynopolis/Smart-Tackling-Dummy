/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "IndicatorLight.h"

IndicatorLight::IndicatorLight(uint8_t led_num, double (*value)(void), double min_val, double max_val, bool is_absolute, bool is_inverted):
StatusLight(led_num), value(value), min_val(min_val), max_val(max_val), is_absolute(is_absolute), is_inverted(is_inverted){}

void IndicatorLight::update(){
    this->interpolate(this->get_percent(), this->low_color, this->high_color, this->current_color);
}

uint8_t * IndicatorLight::interpolate(uint8_t percent, uint8_t * low_color, uint8_t * high_color, uint8_t * output_color){
    output_color[0] = (uint8_t) (low_color[0] + (high_color[0] - low_color[0]) * percent / 100);
    output_color[1] = (uint8_t) (low_color[1] + (high_color[1] - low_color[1]) * percent / 100);
    output_color[2] = (uint8_t) (low_color[2] + (high_color[2] - low_color[2]) * percent / 100);
    return output_color;
}

uint8_t IndicatorLight::get_percent(){
    double percent = this->value() / this->max_val;
    if(this->is_absolute) percent = abs(percent);
    if(this->is_inverted) percent = 1 - percent;
    return (uint8_t) (percent * 100);
}

void IndicatorLight::print(){
    Serial.print("LED: ");
    Serial.print(this->led_num);
    Serial.print(" Color: ");
    Serial.print(this->current_color[0]);
    Serial.print(", ");
    Serial.print(this->current_color[1]);
    Serial.print(", ");
    Serial.println(this->current_color[2]);
}