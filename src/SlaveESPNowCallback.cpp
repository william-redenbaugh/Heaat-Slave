/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
Callback stuff for the espnow can be found here!
*/
#include "SlaveESPnow.h"

// deregisters sending callback!
void ESPNowSlave::disable_send_callback(){
  err = esp_now_unregister_send_cb();
  if(err == !ESP_OK){
    #ifdef DEBUG
    printf("ESP couldn't deregister callback!");
    #endif
  }
}

void ESPNowSlave::disable_receive_callback(){
  // deregiesters callback!
  err = esp_now_unregister_recv_cb();
  if(err == !ESP_OK){
    #ifdef DEBUG
    printf("ESP couldn't deregister callback!");
    #endif
  }
}

void ESPNowSlave::attach_receive_callback(esp_now_recv_cb_t callback){
  // error checks adding espnow callback!
  err = esp_now_register_recv_cb(callback);
  if(err == !ESP_OK){
    #ifdef DEBUG
    printf("ESP couldn't attach a recieve callback, please check to see if ESPNOW is initialized");
    #endif
  }
}
