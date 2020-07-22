/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
Fan Class of sorts can be found here!
*/
#ifndef _FAN_H
#define _FAN_H

#include "LEDPWM.h"

class Fan{
  public: 
    PwmLed heatsink_fan; 
    PwmLed external_fan;
    PwmLed external_fan_two;
    
    void begin(gpio_num_t gpio_pin, gpio_num_t gpio_pin_two, gpio_num_t gpio_pin_three);
};

#endif
