/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
RGB control stuff can be found here!
*/

#include "RGB_control.h"
void RGBControl::begin(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin){
  // initialize and start pin
   red_led.begin(5000, 1, true);
   red_led.attach_pin(1, red_pin, 0, 0);
   
   // initialize and start pin
   green_led.begin(5000, 2, true);
   green_led.attach_pin(2, green_pin, 0, 0);

   blue_led.begin(5000, 3, true);
   blue_led.attach_pin(3, blue_pin, 0, 0);
   ledc_fade_func_install(0);
}

void RGBControl::set(uint8_t red, uint8_t green, uint8_t blue){
  
  this->red_led.set_fade(this->convert(red));
  this->green_led.set_fade(this->convert(green));
  this->blue_led.set_fade(this->convert(blue));  
}

void RGBControl::set_brightness(uint8_t brightness){
  if(brightness > 32){
    brightness = 32;   
  }
  this->brightness = brightness;
}

uint32_t RGBControl::convert(uint8_t val){
  int value = val * brightness - 1; 
  if(value < 0){
    value = 0;
  }
  return value;
}

void RGBControl::fade(uint8_t red, uint8_t green, uint8_t blue, int time){
  red_led.animate_fade(this->convert(red), time);
  green_led.animate_fade(this->convert(green), time);
  blue_led.animate_fade(this->convert(blue), time);  
}

void RGBControl::fade_kelvin(int kelvin, int time){
  double adj_temp = kelvin/100;
  // first we need to deal with the Red calculation!
  // red_cutoff!
  if(adj_temp <= 66){
    this->red = 255;     
  }
  // some mathematical calculations!
  else{
    double adj_red = (adj_temp  - 60); 
    adj_red = 329.698727446 * (pow(adj_red, -0.1332047592));
    
    if(adj_red > 255){
      adj_red = 255;   
    }
    if(adj_red < 0){
      adj_red = 0;   
    }
    this->red = uint8_t(adj_red);
  }

  // calulcate green
  double adj_green; 
  if(adj_temp <= 66){
    adj_green = adj_temp;
    adj_green = ((log(adj_green)) * 99.4708025861) - 161.1195681661;
  }
  else{
     adj_green = adj_temp - 60;
     adj_green = 288.1221695283 * (pow(adj_green, -0.0755148492));
  }
  // more math and simple logic here!
  if(adj_green > 255){
    adj_green = 255;   
  }
  if(adj_green < 0){
    adj_green = 0;   
  }
  this->green = uint8_t(adj_green);

  if(adj_temp >= 66){
      this->blue = 255; 
  }
  else{
  // finishing math and computer logic here...
    if(adj_temp <= 19){
      this->blue = 0;   
    }  
    else{
        double adj_blue = adj_temp - 10; 
        adj_blue = ((log(adj_blue))  * 138.5177312231) - 305.0447927307;
        if(adj_blue > 255){
          adj_blue = 255;   
        }
        if(adj_blue < 0){
          adj_blue = 0;   
        }
        this->blue = uint8_t(adj_blue);
    }
  }
  this->fade(this->red, this->green, this->blue, time); 
}

void RGBControl::set_kelvin(int kelvin){

  double adj_temp = kelvin/100;
  // first we need to deal with the Red calculation!
  // red_cutoff!
  if(adj_temp <= 66){
    this->red = 255;     
  }
  // some mathematical calculations!
  else{
    double adj_red = (adj_temp  - 60); 
    adj_red = 329.698727446 * (pow(adj_red, -0.1332047592));
    
    if(adj_red > 255){
      adj_red = 255;   
    }
    if(adj_red < 0){
      adj_red = 0;   
    }
    this->red = uint8_t(adj_red);
  }

  // calulcate green
  double adj_green; 
  if(adj_temp <= 66){
    adj_green = adj_temp;
    adj_green = ((log(adj_green)) * 99.4708025861) - 161.1195681661;
  }
  else{
     adj_green = adj_temp - 60;
     adj_green = 288.1221695283 * (pow(adj_green, -0.0755148492));
  }
  // more math and simple logic here!
  if(adj_green > 255){
    adj_green = 255;   
  }
  if(adj_green < 0){
    adj_green = 0;   
  }
  this->green = uint8_t(adj_green);

  if(adj_temp >= 66){
      this->blue = 255; 
  }
  else{
  // finishing math and computer logic here...
    if(adj_temp <= 19){
      this->blue = 0;   
    }  
    else{
        double adj_blue = adj_temp - 10; 
        adj_blue = ((log(adj_blue))  * 138.5177312231) - 305.0447927307;
        if(adj_blue > 255){
          adj_blue = 255;   
        }
        if(adj_blue < 0){
          adj_blue = 0;   
        }
        this->blue = uint8_t(adj_blue);
    }
  }
  this->set(this->red, this->green, this->blue);  
}

void RGBControl::fade_next_kelvin(){
  this->fade_kelvin(this->kelvin, 80);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}

void RGBControl::set_next_rgb(){
  this->fade(this->next_r, this->next_g, this->next_b, 80);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}

void RGBControl::fade_next_kelvin_speed(){
  this->fade_kelvin(this->kelvin, int(this->next_speed * .8));
  vTaskDelay(this->next_speed / portTICK_PERIOD_MS);
}

void RGBControl::fade_next_rgb(){
  this->fade(this->next_r, this->next_g, this->next_b, int(this->next_speed * .8));
  vTaskDelay(this->next_speed / portTICK_PERIOD_MS);
}