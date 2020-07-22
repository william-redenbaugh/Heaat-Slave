/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
RGB library and definitions can be found here!
*/
#ifndef RGB_CONTROL_H
#define RGB_CONTROL_H

#include "LEDPWM.h"
#include <cmath>

class RGBControl{
  public: 
    void begin(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin);
    void set(uint8_t red, uint8_t green, uint8_t blue);
    void fade(uint8_t red, uint8_t green, uint8_t blue, int time); 
    void set_kelvin(int kelvin);
    void set_brightness(uint8_t brightness); 
    uint32_t convert(uint8_t val);

    void fade_kelvin(int kelvin, int time);
    int kelvin = 3000;
    
    PwmLed red_led; 
    PwmLed green_led; 
    PwmLed blue_led; 
    int previous_kelvin = 1200; 
    
    uint8_t next_r = 0; 
    uint8_t next_g = 0; 
    uint8_t next_b = 0; 

    int next_speed = 200; 

    void fade_next_kelvin();
    void set_next_rgb();
    void fade_next_kelvin_speed();
    void fade_next_rgb();
        

  private:
    uint8_t red = 0; 
    uint8_t green = 0; 
    uint8_t blue = 0; 

    // max brightness is 32!
    uint8_t brightness = 32; 

};

#endif 
