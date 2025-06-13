/**
 * @file IRCom.cpp
 * @brief IR remote control class
 * @author Quinn Henthorne. Email: henth013@d.umn.edu, Phone: 763-656-8391
 * @version 1.0
 * @date 03-29-23
*/

#include "IRCom.h"

IRCom::IRCom(uint8_t pin) {
    this->pin = pin;
    receiver = new IRrecv(pin);
}

void IRCom::init() {
    receiver->enableIRIn();
}

void IRCom::update() {
    decode_results results;
    if (receiver->decode(&results)) {
        if(codesAvailableCount < maxCodes) codesAvailableCount++;
        switch (results.command) {
            case 0x44:{ // A button
                pushCode(toggleRecord);
                // Serial.println("A button pressed");
                break;
            }
            case 0x0C:{ // left arrow
                pushCode(leftLight);
                // Serial.println("Left arrow pressed");
                break;
            }
            case 0x5E:{ // right arrow
                pushCode(rightLight);
                // Serial.println("Right arrow pressed");
                break;
            }
            case 0x40:{ // b button
                pushCode(reset);
                //Serial.println("B button pressed");
                break;
            }
            // these codes are added, but don't do anything. Could be used to extend functionality later
            case 0x19:{ // up arrow
                break;
            }
            case 0x1C:{ // down arrow
                break;
            }
            case 0x43:{ // C button
                break;
            }
            case 0x46:{ // D button
                break;
            }
            case 0x18:{ // Center button
                break;
            }
            case 0x00:{ // no button. This state is sometimes occurs on accident. Don't use it.
                break;
            }
            default:{
                Serial.print("Unknown IR Code: ");
                Serial.println(results.command, HEX);
                codesAvailableCount--;
                break;
            }
        }
        receiver->resume();
    }
}

IRCode IRCom::popCode() {
    // if there are codes in the stack, return the oldest one
    if(codesAvailableCount > 0) {
        codesAvailableCount--;
        return results[codesAvailableCount];
    }
    // if there are no codes in the stack, return nothing
    else {
        return nothing;
    }

}

void IRCom::pushCode(IRCode code) {
    // if there is room in the stack, add the code to the stack
    if(codesAvailableCount < maxCodes) {
        results[codesAvailableCount] = code;
        codesAvailableCount++;
    }
}

uint8_t IRCom::codesAvailable() {
    return codesAvailableCount;
}

