#ifndef _SLAVEESPNOW_H
#define _SLAVEESPNOW_H
/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
Slave ESPNow stuff can be found here. Class and definitiosn are defined here!
*/
#include "esp_event.h"
#include "tcpip_adapter.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include <stdio.h>
#include <string.h>
#include "freertos/task.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#define ESP_CHANNEL 3
#define DEBUG

class ESPNowSlave{
  public: 
    // starts system
    void begin();

    // starts receive callback so we can checkout messages!
    void attach_receive_callback(esp_now_recv_cb_t callback);
    
    // starts up wifi antenna
    void start_wifi();
    
    // sets up broadcasting! 
    void setup_broadcast(uint8_t channel);
    
    // broadcast's message!
    void broadcast(uint8_t *data, int size);

    // disables dreceiving callbc!
    void disable_receive_callback();
    
    // disables sending callback
    void disable_send_callback();

    // enables pairing mode!
    void start_pairing_mode();
    
    // checks a pair
    bool check_pair(const uint8_t *mac_addr, const uint8_t *data, int data_len);

    // set mac address
    void set_master(const uint8_t *mac_add, int channel); 

    // send a message to master. 
    void send_master_message(uint8_t* data, int size); 

    // if the master is selected mac address!
    bool if_master(const uint8_t* mac_addr); 

    // error checking
    esp_err_t err;   
    
    // broadcasting to all!
    esp_now_peer_info_t broadcast_peer;

    // master device handler
    esp_now_peer_info_t master_peer;

    bool not_paired = true; 
};

#endif 
