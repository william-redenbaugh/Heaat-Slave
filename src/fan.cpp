/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
Fan startup function can be found here!
*/

#include "fan.h"

void Fan::begin(gpio_num_t gpio_pin, gpio_num_t gpio_pin_two, gpio_num_t gpio_pin_three){
  this->heatsink_fan.begin(5000, 4, false);
  this->heatsink_fan.attach_pin(4, gpio_pin, 0, 0);
  this->heatsink_fan.set_fade(8192);
  
  this->external_fan.begin(5000, 5, false);
  this->external_fan.attach_pin(5, gpio_pin_two, 0, 0);   
  this->external_fan.set_fade(8192);

  /*
  this->external_fan_two.begin(22000, 6, false);
  this->external_fan_two.attach_pin(6, gpio_pin_three, 0, 0);
  this->external_fan.set_fade(250);
  */
}
