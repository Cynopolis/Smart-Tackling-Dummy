/**
 * @file IRCom.cpp
 * @brief control panel class
 * @author Quinn Henthorne. Email: henth013@d.umn.edu, Phone: 763-656-8391
 * @version 1.0
 * @date 03-29-23
*/

#pragma once

#include "LEDStrip.h"

#define USE_IR
#include "IRCom.h"

class ControlPanel{
    public:
        ControlPanel(uint8_t leftButtonPin, uint8_t rightButtonPin, uint8_t recordButtonPin, uint8_t resetButtonPin);
        ControlPanel(uint8_t leftButtonPin, uint8_t rightButtonPin, uint8_t recordButtonPin, uint8_t resetButtonPin, LEDStrip * led, IRCom * ir);

        /**
         * @brief Initialize the control panel
         */
        void init();
        
        /**
         * @brief Update the state of the control panel
         */
        void update();

        /**
         * @brief Set the led obect
         * @param led 
         * @post The internal pointer of the LED object will be set to the passed in pointer
         */
        void setLED(LEDStrip *led);
        
        /**
         * @brief Set the IR remote pointer
         * @param ir
         * @post The internal pointer of the IRremote object will be set to the passed in pointer
         */
        void setIR(IRCom *ir);
        
        /**
         * @brief Get an array of the state of the buttons
         * @return a 4 entry array of bools, true if the button has been pressed. The order is left, right, record, reset
         */
        bool * getButtonStates();

        /**
         * @brief Set the state of a button
         * @param button the index of the button to set
         * @param state the state to set the button to
         */
        void setButtonState(uint8_t button_idx, bool state);


    private:
        LEDStrip * indic_lights = nullptr;
        IRCom * ir_remote = nullptr;
        uint8_t left_button_pin;
        uint8_t right_button_pin;
        uint8_t record_button_pin;
        uint8_t reset_button_pin;
        unsigned long left_button_timer = 0;
        unsigned long right_button_timer = 0;
        unsigned long record_button_timer = 0;
        unsigned long reset_button_timer = 0;

        static const uint8_t button_num = 4;

        // left, right, record reset. In that order
        bool button_states[button_num] = {false};
};