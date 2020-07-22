/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
Main and application level code can be found here!
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_err.h"
#include "freertos/event_groups.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "SlaveESPnow.h"
#include "RGB_control.h"
#include "fan.h"

#define CHANNEL 3
#define SYSTEM_COMMANDS 1

struct Main{
      
  // pointer reference to slave in the H E A P
  ESPNowSlave* slave = new ESPNowSlave();
  RGBControl led;
  Fan fan;
  TaskHandle_t pairing_task_handler; 

  uint8_t lat_data_len = 0; 
  EventGroupHandle_t anim_wait;
  const int start_fade_kelvin = BIT0;
};

Main sys;

void run_led(const uint8_t *mac_addr, const uint8_t *data, int data_len){
  sys.lat_data_len = data_len; 
  if(!sys.slave->if_master(mac_addr)) return; 
  switch(sys.lat_data_len){
    case SYSTEM_COMMANDS:
      if(data[0] == 69){
        printf("device has been instructed to reset!.. rebooting... \n");
        vTaskDelay(100 / portTICK_PERIOD_MS);
        esp_restart();
        // no need to break since whole machine is restarting!
      }
      else if(data[0] <= 32){
        sys.led.set_brightness(data[0]);
      }
    // only getting bitshifted kelvin data
    case 2:
      sys.led.kelvin = ((data[0] << 8) | data[1]);
      printf("Fading to Kelvin Value: %d \n", sys.led.kelvin);
      break;
    
    // getting rgb data
    case 3:
      sys.led.next_r = data[0];
      sys.led.next_g = data[1];
      sys.led.next_b = data[2];
      printf("Fading to the next rgb value: %d %d %d \n", data[0], data[1], data[2]);
      break;

    // geting kelvin data at a defined speed
    case 4: 
      sys.led.kelvin = ((data[0] << 8) | data[1]);
      sys.led.next_speed = ((data[2] << 8) | data[3]);
      printf("Fading to Kelvin Value: %d and should be done in %d milliseconds! \n", sys.led.kelvin, sys.led.next_speed);
      break;
    
    // getting RGB data at a defined speed
    case 5: 
      sys.led.next_r = data[0];
      sys.led.next_g = data[1]; 
      sys.led.next_b = data[2];
      sys.led.next_speed = ((data[3] << 8) | data[4]);
      printf("Fading to the next rgb value: %d %d %d  and should be done in %d milliseconds\n", data[0], data[1], data[2], sys.led.next_speed);
      break;
      
    default:
      break;
    }

  // tells the main thread that animation may begin!
  xEventGroupSetBits(sys.anim_wait, sys.start_fade_kelvin);
}

// callback when data is recv from Master
void pair_receive_callback(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  
  // checks for a pair!
  if(sys.slave->check_pair(mac_addr, data, data_len)){
    sys.slave->set_master(mac_addr, CHANNEL);
    // suspends and deletes the pairing task handler!
    vTaskSuspend(sys.pairing_task_handler);
    vTaskDelete(sys.pairing_task_handler);
    
    // starts running the main thread!
    sys.slave->attach_receive_callback(run_led);
    // sets a really low brightnes value for now!
    sys.led.set_brightness(10);
    sys.led.fade_kelvin(4200, 1000);
  }
}

void pairing_mode(void* parameters){
  // pairing mode flashing animation
  sys.led.set_brightness(10);
  while(1){
    sys.led.fade(0, 0, 255, 1000);
    vTaskDelay(TickType_t(3000 / portTICK_PERIOD_MS));
    sys.led.fade(0, 255, 0, 1000);
    vTaskDelay(TickType_t(3000 / portTICK_PERIOD_MS));
    sys.led.fade(255, 0, 0, 1000);
    vTaskDelay(TickType_t(3000 / portTICK_PERIOD_MS));

    sys.led.fade(0, 0, 0, 1000);
    vTaskDelay(TickType_t(3000 / portTICK_PERIOD_MS));
  }  
}

void setup() {

  sys.fan.begin(GPIO_NUM_5, GPIO_NUM_16, GPIO_NUM_19);
  sys.led.begin(GPIO_NUM_14, GPIO_NUM_22, GPIO_NUM_23);
  //fade(0, 0, 255, 1000);
  
  sys.anim_wait = xEventGroupCreate();
  // starts serial because why not
  // 
  //Serial.begin(115200);
  // starts slave stuff
  sys.slave->begin();
  // attaches a receiving callback
  sys.slave->attach_receive_callback(pair_receive_callback);
  // we gotta setup broadcast first and foremost!
  sys.slave->setup_broadcast(CHANNEL);
  vTaskDelay(TickType_t(2000 / portTICK_PERIOD_MS));
  sys.slave->start_pairing_mode();
  
  xTaskCreate(
                    pairing_mode,   /* Function to implement the task */
                    "pairing_task", /* Name of the task */
                    1200,      /* Stack size in words */
                    NULL,       /* Task input parameter */
                    40,          /* Priorit y of the task */
                    &sys.pairing_task_handler);
  sys.led.set_brightness(10);
}

// nothing running in the loop, so lets ignore this!
void loop() {
  
  // waiting for the next callback!
  xEventGroupWaitBits(sys.anim_wait, sys.start_fade_kelvin, true, true, portMAX_DELAY);
  
  // depending on the size of the packet, we may process the data, and
  // output different stuff!
  switch(sys.lat_data_len)
  {
  // just fading kelvin values
  case 2:
    sys.led.fade_next_kelvin();
    break;
  
  case 3:
    // just fading RGB values 
    sys.led.set_next_rgb();
    break;

  case 4: 
    // fading kelvin values at a defined speed
    sys.led.fade_next_kelvin_speed();
    break;
  
  case 5: 
    // fading rgb values at a defined speed 
    sys.led.fade_next_rgb();
    break;
    
  default:
    break;
  }

  uint8_t val = 255; 
  sys.slave->send_master_message(&val, 1);
  printf("Light has completed fading \n");
  
}

// sets up esp flash!
esp_err_t setup_nvs_flash(){
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
  }
  return err; 
}

extern "C" int app_main(void){
  ESP_ERROR_CHECK(setup_nvs_flash());
  setup();
  // runn the loop!
  for(;;){
    loop();
  }
  // not really a reason to return...
  return 0; 
}