/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#include "LEDStrip.h"

LEDStrip::LEDStrip(uint8_t pin, StatusLight * status_lights[], unsigned int status_lights_length)
: led_strip_pin(pin), status_lights(status_lights), status_light_count(status_lights_length){
    this->pixels = new Adafruit_NeoPixel(status_lights_length, pin, NEO_GRB + NEO_KHZ800);
}

void LEDStrip::init(){
    this->pixels->begin();
}

void LEDStrip::update(){
    for(auto i = 0; i < this->status_light_count; i++){
        this->status_lights[i]->update();
        uint8_t * current_color = this->status_lights[i]->get_current_color();
        this->pixels->setPixelColor(i, this->pixels->Color(current_color[0], current_color[1], current_color[2]));
    }
    this->pixels->show();
}

void LEDStrip::print(){
    for(auto i = 0; i < this->status_light_count; i++){
        this->status_lights[i]->print();
    }
}