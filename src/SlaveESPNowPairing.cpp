/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
pairing stuff for the espnow 
*/
#include "SlaveESPnow.h"

// we check if stuff as paired
bool ESPNowSlave::check_pair(const uint8_t *mac_addr, const uint8_t *data, int data_len){
  if(data_len == 6){
    uint8_t mac[6] = {0}; 
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    // if it's our own mac address, we chillin
    if(data[0] == mac[0] && data[1] == mac[1] && data[2] == mac[2] && data[3] == mac[3] && data[4] == mac[4] && data[5] == mac[5]){
      #ifdef DEBUG
      printf("We have paired tO: %02x:%02x:%02x:%02x:%02x:%02x ", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]); 
      #endif
      this->not_paired = false;   
      // stops the pairing callback
      this->disable_receive_callback();

      return true;
    }
  }

  return false; 
}

// sets and pairs the master!
void ESPNowSlave::set_master(const uint8_t *mac_add, int channel){
  for(int i = 0; i < 6; i++){
    this->broadcast_peer.peer_addr[i] = mac_add[i]; 
   }
   this->broadcast_peer.channel = channel;
   this->broadcast_peer.encrypt = 0; 

  esp_now_peer_info_t *peer = &broadcast_peer;
  uint8_t* peer_addr = broadcast_peer.peer_addr;
  // if check if peer exists
  bool exist = esp_now_is_peer_exist(peer_addr);
  
  #ifdef DEBUG
  // prints out the slaves ID by mac address
  printf("Slave ID: \n");
  for (int ii = 0; ii < 6; ++ii ) {
    printf("%d ",broadcast_peer.peer_addr[ii]);
    if (ii != 5) printf(":");
  }
  printf(" Status: ");
  #endif

  // if device is paired, we let the debugger know!
  if(exist){
    #ifdef DEBUG
    printf("Device Already Paired!\n");
    #endif  
  }
  // if device aint parieds
  else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(peer);
      if (addStatus == ESP_OK) {
        // Pair success
        #ifdef DEBUG
        printf("Pair success\n");
        #endif  
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        #ifdef DEBUG
        printf("ESPNOW Not Init\n");
        #endif  
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        #ifdef DEBUG
        printf("Add Peer - Invalid Argument\n");
        #endif  
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        #ifdef DEBUG
        printf("Peer list full\n");
        #endif  
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        #ifdef DEBUG
        printf("Out of memory\n");
        #endif  
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        #ifdef DEBUG
        printf("Peer Exists\n");
        #endif  
      } else {
        #ifdef DEBUG
        printf("Not sure what happened\n");
        #endif  
      }
      // wait 100ms per device. 
      vTaskDelay(TickType_t(100 / portTICK_PERIOD_MS));  
    }
}

void pairing_mode_task(void *parameters){
  ESPNowSlave *obj = static_cast<ESPNowSlave*>(parameters);
  for(;;){
    if(obj->not_paired){
      uint8_t data[4] = {20, 30, 40, 40}; 
      uint8_t *buff = data; 
      obj->broadcast(buff, sizeof(data));  
    }
    else break;
    // update every seconds 
    vTaskDelay(TickType_t(1000 / portTICK_PERIOD_MS));
  }
  vTaskDelete(NULL);
}

void ESPNowSlave::start_pairing_mode(){
  // creates a broadcasting task!
  xTaskCreate(pairing_mode_task, "Pairing Task", 1200,(void*)this, 40, NULL);
}