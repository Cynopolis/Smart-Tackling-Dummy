#include "ErrorLight.h"

ErrorLight::ErrorLight(uint8_t led_num) : StatusLight(led_num){
    set_animation(SOLID, false);
}

void ErrorLight::set_animation(Animation animation, bool is_high){
    this->animation = animation;
    this->is_high = is_high;
}

void ErrorLight::update(){
    switch(animation){
        case SOLID:
            if(is_high){
                copy_array(high_color, current_color);
            }
            else{
                copy_array(low_color, current_color);
            }
            break;
        case BLINK_ONCE:
            blink_once();
            break;
        case BLINK_TWICE:
            blink_twice();
            break;
        case BLINK_THRICE:
            blink_thrice();
            break;
        case PULSE:
            pulse();
            break;
    }
}

void ErrorLight::blink_once(){
    // this animation takes 2 seconds to complete
    uint8_t off[] = {0,0,0};
    unsigned long blink_time = 500;
    unsigned long pause_time = 1500;
    unsigned long current_time = millis();
    if(current_time - timer < blink_time){
        if(is_high){
            copy_array(high_color, current_color);
        }
        else{
            copy_array(low_color, current_color);
        }
    }
    else if(current_time - timer < blink_time + pause_time){
        copy_array(off, current_color);
    }
    else{
        timer = millis();
    }
}

void ErrorLight::blink_twice(){
    // this animation takes 3 seconds to complete
    uint8_t off[] = {0,0,0};
    unsigned long blink_time = 500;
    unsigned long pause_time = 500;
    unsigned long current_time = millis();

    if(current_time - timer < blink_time){
        if(is_high){
            copy_array(high_color, current_color);
        }
        else{
            copy_array(low_color, current_color);
        }
    }
    else if(current_time - timer < blink_time + pause_time){
        copy_array(off, current_color);
    }
    else if(current_time - timer < 2*blink_time + pause_time){
        if(is_high){
            copy_array(high_color, current_color);
        }
        else{
            copy_array(low_color, current_color);
        }
    }
    else if(current_time - timer < 2*blink_time + 2*pause_time){
        copy_array(off, current_color);
    }
    else{
        timer = millis();
    }
}

void ErrorLight::blink_thrice(){
    // this animation takes 4 seconds to complete
    uint8_t off[] = {0,0,0};
    unsigned long blink_time = 500;
    unsigned long pause_time = 500;
    unsigned long current_time = millis();

    if(current_time - timer < blink_time){
        if(is_high){
            copy_array(high_color, current_color);
        }
        else{
            copy_array(low_color, current_color);
        }
    }
    else if(current_time - timer < blink_time + pause_time){
        copy_array(off, current_color);
    }
    else if(current_time - timer < 2*blink_time + pause_time){
        if(is_high){
            copy_array(high_color, current_color);
        }
        else{
            copy_array(low_color, current_color);
        }
    }
    else if(current_time - timer < 2*blink_time + 2*pause_time){
        copy_array(off, current_color);
    }
    else if(current_time - timer < 3*blink_time + 2*pause_time){
        if(is_high){
            copy_array(high_color, current_color);
        }
        else{
            copy_array(low_color, current_color);
        }
    }
    else if(current_time - timer < 3*blink_time + 3*pause_time){
        copy_array(off, current_color);
    }
    else{
        timer = millis();
    }
}

void ErrorLight::pulse(){
    // this animation takes 1 second to complete
    unsigned long pulse_time = 1000;
    unsigned long current_time = millis();
    if(current_time - timer < pulse_time){
        float percent = (float)(current_time - timer) / pulse_time;
        for(int i = 0; i < 3; i++){
            current_color[i] = (uint8_t)(percent * high_color[i]);
        }
    }
    else{
        timer = millis();
    }
}

void ErrorLight::print(){
    Serial.print("LED NUM: ");
    Serial.println(led_num);
    Serial.print("ANIMATION: ");
    Serial.print(animation);
    Serial.print(" IS HIGH: ");
    Serial.print(is_high);
    Serial.print(" CURRENT COLOR: ");
    Serial.print(current_color[0]);
    Serial.print(", ");
    Serial.print(current_color[1]);
    Serial.print(", ");
    Serial.println(current_color[2]);
}