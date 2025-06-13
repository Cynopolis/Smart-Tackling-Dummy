/**
 * @file IRCom.cpp
 * @brief Control panel class
 * @author Quinn Henthorne. Email: henth013@d.umn.edu, Phone: 763-656-8391
 * @version 1.0
 * @date 03-29-23
*/
#include "ControlPanel.h"
#include <Arduino.h>

ControlPanel::ControlPanel(uint8_t leftButtonPin, uint8_t rightButtonPin, uint8_t recordButtonPin, uint8_t resetButtonPin) :
left_button_pin(leftButtonPin), 
right_button_pin(rightButtonPin), 
record_button_pin(record_button_pin), 
reset_button_pin(resetButtonPin){}


ControlPanel::ControlPanel(uint8_t leftButtonPin, uint8_t rightButtonPin, uint8_t recordButtonPin, uint8_t resetButtonPin, LEDStrip * led, IRCom * ir) :
left_button_pin(leftButtonPin),
right_button_pin(rightButtonPin),
record_button_pin(record_button_pin),
reset_button_pin(resetButtonPin),
indic_lights(led),
ir_remote(ir){}

void ControlPanel::init(){
    Serial.println("Initializing control panel");
    pinMode(this->left_button_pin, INPUT_PULLDOWN);
    pinMode(this->right_button_pin, INPUT_PULLDOWN);
    pinMode(this->record_button_pin, INPUT_PULLDOWN);
    pinMode(this->reset_button_pin, INPUT_PULLDOWN);

    if(this->indic_lights != nullptr){
        Serial.println("Initializing LED strip");
        this->indic_lights->init();
    }
    #ifdef USE_IR
    if(this->ir_remote != nullptr){
        Serial.println("Initializing IR remote");
        this->ir_remote->init();
    }
    #endif
}

void ControlPanel::update(){
    #ifdef USE_IR
    // get the IR remote readings first
    this->ir_remote->update();

    bool new_states[4] = {0, 0, 0, 0};

    while(this->ir_remote->codesAvailable() > 0){
        IRCode code = this->ir_remote->popCode();
        switch(code){
            case leftLight:
                new_states[0] |= 1;
                break;
            case rightLight:
                new_states[1] |= 1;
                break;
            case toggleRecord:
                new_states[2] |= 1;
                break;
            case reset:
                new_states[3] |= 1;
                break;
            default:
                break;
        }
    }
    #endif

    // we invert the digital read because of the pullup resistor
    new_states[0] |= digitalRead(this->left_button_pin);
    new_states[1] |= digitalRead(this->right_button_pin);
    new_states[2] |= digitalRead(this->record_button_pin);
    new_states[3] |= digitalRead(this->reset_button_pin);

    // debounce all of the buttons
    if(new_states[0] != this->button_states[0] && millis() - left_button_timer > 100){
        this->button_states[0] = new_states[0];
        left_button_timer = millis();
    }
    if(new_states[1] != this->button_states[1] && millis() - right_button_timer > 100){
        this->button_states[1] = new_states[1];
        right_button_timer = millis();
    }
    if(new_states[2] != this->button_states[2] && millis() - record_button_timer > 100){
        this->button_states[2] = new_states[2];
        record_button_timer = millis();
    }
    if(new_states[3] != this->button_states[3] && millis() - reset_button_timer > 100){
        this->button_states[3] = new_states[3];
        reset_button_timer = millis();
    }

    // update the LED lights
    this->indic_lights->update();
}

void ControlPanel::setLED(LEDStrip * led){
    this->indic_lights = led;
}

#ifdef USE_IR
void ControlPanel::setIR(IRCom * ir){
    this->ir_remote = ir;
}
#endif

bool * ControlPanel::getButtonStates(){
    return this->button_states;
}

void ControlPanel::setButtonState(uint8_t button_idx, bool state){
    // don't try to write past the end of the array
    if(button_idx >= button_num) return;
    button_states[button_idx] = state;
}