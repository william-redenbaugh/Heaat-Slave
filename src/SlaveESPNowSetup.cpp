/*
Author: William Redenbaugh
Last Edit Date: 6/6/19
Notes:
Random ESPNowSlave setup and callback functions can be found here!
*/
#include "SlaveESPnow.h"


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // just the default callback that's set when you start
  // if debug mode is enabled, the  we read all the data sent from the packet
  #ifdef DEBUG
  printf("Last Packet Sent to: %02x:%02x:%02x:%02x:%02x:%02x ", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  printf(" Last Packet Send Status: ");
  printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success \n" : "Delivery Fail \n");
  #endif
}

void ESPNowSlave::start_wifi(){
  // starts tcp over ip adapter
  tcpip_adapter_init();
  // creates event loop
  //ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK( esp_wifi_start());

  /* In order to simplify example, channel is set after WiFi started.
   * This is not necessary in real application if the two devices have
   * been already on the same channel.
   */
  ESP_ERROR_CHECK( esp_wifi_set_channel(ESP_CHANNEL, WIFI_SECOND_CHAN_ABOVE) );

  // long range would be pretty cool!
  #if CONFIG_ENABLE_LONG_RANGE
  ESP_ERROR_CHECK( esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );
  #endif
  esp_wifi_disconnect();
}

// allows us to broadcast messages, mainly used for pairing purposes
void ESPNowSlave::setup_broadcast(uint8_t channel){
  // sets peer address to 255
  for(int i = 0; i < 6; i++){
    this->broadcast_peer.peer_addr[i] = 255;
  }
  // no encryption please
  this->broadcast_peer.channel = channel; 
  this->broadcast_peer.encrypt = 0; 
  
  esp_err_t addStatus = esp_now_add_peer(&broadcast_peer);
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

  // attaches the default callback
  esp_now_register_send_cb(OnDataSent);
}


// starts wifi and espnow stuff!
void ESPNowSlave::begin(){
  this->start_wifi();
  // try starting espnow a couple times!
  uint8_t count = 0; 
  while(1){
    this->err = esp_now_init(); 
    if(this->err == ESP_OK){
      #ifdef DEBUG
      printf("ESP Initialized properly!\n");
      #endif  
      break;
    }
    #ifdef DEBUG
    printf("Hmm.. something went wrong :( Retrying...\n");
    #endif  
      
    vTaskDelay(TickType_t(100 / portTICK_PERIOD_MS));  
    count++; 
    if(count == 11){
      // if espnow couldn't startup, we restart, just hopefully that will fix the problem
      #ifdef DEBUG
      printf("ESP Couldn't Initialize.. restarting!\n");
      #endif  
      esp_restart();  
    }  
  }
  // attaches the default callback
}
