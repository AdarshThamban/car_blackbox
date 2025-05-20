/*
 * File:   digital_keypad.c
 * Author: Adarsh
 *
 * Created on August 1, 2024, 11:09 AM
 */


#include <xc.h>
#include "digital_keypad.h"

void init_digital_keypad(void)
{
    /* Set Keypad Port as input */
    KEYPAD_PORT_DDR = KEYPAD_PORT_DDR | INPUT_LINES;
}

unsigned char read_digital_keypad(unsigned char mode)
{
    static unsigned char once = 1;
    static unsigned int long_press = 0;
    static unsigned char switch_state = ALL_RELEASED;

    unsigned char key = KEYPAD_PORT & INPUT_LINES;

    if (mode == LEVEL_DETECTION)
    {
        return key;
    }
    else
    {
        // Long/Short press logic for SW4 and SW5
        if (key == SW4 || key == SW5)
        {
            if (key == switch_state)  // key still being held
            {
                long_press++;
                if (long_press > 200)
                {
                    long_press = 0;
                    switch_state = ALL_RELEASED;
                    return (key == SW4) ? LP_SW4 : LP_SW5;
                }
            }
            else if (switch_state == ALL_RELEASED)
            {
                switch_state = key;  // new press started
                long_press = 0;
            }
        }
        else
        {
            if (switch_state == SW4 || switch_state == SW5)
            {
                // key was released before long press threshold
                unsigned char temp = switch_state;
                switch_state = ALL_RELEASED;
                long_press = 0;
                return temp;  // short press
            }
        }

        // State detection for other keys
        if (key != ALL_RELEASED && once && key != SW4 && key != SW5)
        {
            once = 0;
            return key;
        }
        else if (key == ALL_RELEASED)
        {
            once = 1;
        }
    }

    return ALL_RELEASED;
}
