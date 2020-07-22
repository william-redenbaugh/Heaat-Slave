/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
Slave ESPNow stuff can be found here
most notably the random functions that help slave espnow work
*/
#include "SlaveESPnow.h"

// allows us to broadcast data to the general public!
void ESPNowSlave::broadcast(uint8_t *data, int size){  
  const uint8_t *peer_addr = broadcast_peer.peer_addr;
  this->err = esp_now_send(peer_addr, data, size);
  // if it worked fined
  if (this->err == ESP_OK) {
    #ifdef DEBUG
    printf("Data was sent successfully\n");
    #endif 
  }
}

// check to see if dem slaves are there!
bool ESPNowSlave::if_master(const uint8_t *mac_addr){
  return esp_now_is_peer_exist(mac_addr);
}

// sends a message to the master!
void ESPNowSlave::send_master_message(uint8_t* data, int size){
 // sets peer address
    const uint8_t *peer_addr = broadcast_peer.peer_addr; 
    // sends data array of defined size(because pointers)
    this->err = esp_now_send(peer_addr, data, size);
    // if it worked fined
    if (this->err == ESP_OK) {
      #ifdef DEBUG
      printf("Data was sent successfully\n");
      #endif 
    }
    // lets us know what went wrong
    #ifdef DEBUG
      else if (this->err == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      printf("ESPNOW not Init.\n");
      } else if (this->err == ESP_ERR_ESPNOW_ARG) {
      printf("Invalid Argument\n");
      } else if (this->err == ESP_ERR_ESPNOW_INTERNAL) {
      printf("Internal Error\n");
      } else if (this->err == ESP_ERR_ESPNOW_NO_MEM) {
      printf("ESP_ERR_ESPNOW_NO_MEM\n");
      } else if (this->err == ESP_ERR_ESPNOW_NOT_FOUND) {
      printf("Peer not found.\n");
      } else {
      printf("Not sure what happened\n");
    }
    #endif   
}
