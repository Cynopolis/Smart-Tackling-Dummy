/**
 * @author Quinn Henthorne Email: henth013@d.umn.edu Phone: 763-656-8391
 * @date 03-26-2023
 * @brief This is the main file for the concussion detection system
*/

#pragma once
#include "StatusLight.h"

typedef enum{
    SOLID,
    BLINK_ONCE,
    BLINK_TWICE,
    BLINK_THRICE,
    PULSE
}Animation;

class ErrorLight : public StatusLight{
    public:
        ErrorLight(uint8_t led_num);

        void update() override;

        void set_animation(Animation animation, bool is_high);

        void print() override;

    private:
        Animation animation;
        bool is_high; // which color to use for the animation
        unsigned long timer = 0;

        void blink_once();

        void blink_twice();

        void blink_thrice();

        void pulse();
};