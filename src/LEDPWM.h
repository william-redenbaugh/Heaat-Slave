/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
PWM library declarations can be found here!
*/
#ifndef _LEDPWM_H
#define _LEDPWM_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_system.h"
#include <stdio.h>

#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE

#define DEBUG

class PwmLed{
  public:
    void begin(int freq, uint8_t timer, bool hs_mode);

    // setup pins 
    void attach_pin(int channel, gpio_num_t gpio_pin, int duty, int h_point);

    // setup fades with defined fade value
    void set_fade(int fade);

    void set_brightness(uint8_t brightness);

    // method call that animation stuff!
    void animate_fade(int fade, int anim_val);

    // stops/uninstalls led driver stuff!
    void stop();

    // tells machine to change the freq!
    void change_freq(uint32_t freq);
    // timer config object 
    ledc_timer_config_t led_timer;

    void end_delete();
    // channel config object!
    ledc_channel_config_t led_channel;
     
    // frequency of object
    int freq = 0; 
    
    // whether or not high speed mode is enabled
    bool hs_mode = true;
  
    // timer object to define
    ledc_timer_t timer; 
    
    // channel object to define
    ledc_channel_t channel; 

    // gpio_num pin stuff
    gpio_num_t gpio_pin; 
  
    // various varables important to management duty cycle values
    int h_point = 0; 
    int duty = 0; 
    ledc_mode_t speed_mode;

    // error checking shit :)
    esp_err_t err;
};
#endif