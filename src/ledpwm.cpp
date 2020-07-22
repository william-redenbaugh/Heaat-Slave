/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
PWM stuff can be found here, while it's called led pwm, it can also be used to control the fans 
*/
#include "LEDPWM.h"

void PwmLed::stop(){
  err = ledc_stop(this->speed_mode, this->channel, 0);
  if(err == !ESP_OK){
    printf("could not disable led driver IC properly :(");  
  }
}

void PwmLed::end_delete(){
  this->stop();
  delete this;  
}

void PwmLed::begin(int freq, uint8_t timer, bool hs_mode){
  // set duty resolution
  this->led_timer.duty_resolution = LEDC_TIMER_13_BIT;

  // this-> frequency. 
  this->freq = freq; 
  
  // set frequency there
  this->led_timer.freq_hz = this->freq;
  
  // set speed mode. 
  this->hs_mode = hs_mode; 
  if(hs_mode){
    speed_mode = LEDC_HS_MODE;
    this->led_timer.speed_mode = LEDC_HS_MODE;  
  }
  // if not true set low speed mode
  else{
    speed_mode = LEDC_LS_MODE;
    this->led_timer.speed_mode = LEDC_LS_MODE;
  }
   // which timer am I using?
   // lets my use integer values rather than timer enumerators. 
  switch(timer){
    case(0):
      this->timer = LEDC_TIMER_0;
      break;

    case(1):
      this->timer = LEDC_TIMER_1;
      break;

    case(2):
      this->timer = LEDC_TIMER_2;
      break;
      
    case(3):
      this->timer = LEDC_TIMER_3;
      break;
  };
  // set timer here 
  this->led_timer.timer_num = this->timer;
  err = ledc_timer_config(&led_timer);
  if(err == !ESP_OK){
    printf("led channel config went wrong!");  
  }
}

void PwmLed::attach_pin(int channel, gpio_num_t gpio_pin, int duty, int h_point){
  // set the right gpio pin into gpio pin object
  
   // go through the differnet channel
  switch(channel){
    case(0):
      this->channel = LEDC_CHANNEL_0;
      break;
    case(1):
      this->channel = LEDC_CHANNEL_1;
      break;
    case(2):
      this->channel = LEDC_CHANNEL_2;
      break;
    case(3):
      this->channel = LEDC_CHANNEL_3;
      break;
    case(4):
      this->channel = LEDC_CHANNEL_4;
      break;
    case(5):
      this->channel = LEDC_CHANNEL_6;
      break;
    case(6):
      this->channel = LEDC_CHANNEL_6;
      break;  
    case(7):
      this->channel = LEDC_CHANNEL_7;
      break;
  };  
  this->led_channel.channel = this->channel;

  // setting the right duty cycle
  this->led_channel.duty = duty; 
 
  // tell the chanel config(MUX) chip which gpio pin to use
  this->gpio_pin = gpio_pin;
  this->led_channel.gpio_num = this->gpio_pin;

  // set speed mode!
  this->led_channel.speed_mode = speed_mode;
  
  // defines the hpoint stuff
  this->h_point = h_point; 
  this->led_channel.hpoint = this->h_point;

  // select the right timer for the mux
  this->led_channel.timer_sel = this->timer;

  // configurate the led channel then install it all :)
  err = ledc_channel_config(&this->led_channel);
  if(err == !ESP_OK){
    printf("led channel config went wrong!");  
  }
}

void PwmLed::set_fade(int fade){
  // setup duty cycle
  ledc_set_duty(this->speed_mode, this->channel, fade);
  // update that shit :)
  ledc_update_duty(this->speed_mode, this->channel);
}

void PwmLed::animate_fade(int fade, int anim_val){
  // setup the animation stuff!
  ledc_set_fade_with_time(this->speed_mode, this->channel, fade, anim_val);
  // start animation, but run in background and don't "block" the app code
  ledc_fade_start(this->speed_mode, this->channel, LEDC_FADE_NO_WAIT);
}

void PwmLed::change_freq(uint32_t freq){
  ledc_set_freq(this->speed_mode , this->timer, freq);
}
