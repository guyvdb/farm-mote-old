#include <stdio.h>
#include "esp_system.h"
#include "id.h"

#include "../command/command.h"




static void cmd_id(char *argv[], int argc) {
  esp_err_t err;
  uint8_t address;
  
  printf("id command called\n");

  err = esp_base_mac_addr_get(&address);

  printf("address: %d\n", address);

  /// esp_err_t esp_base_mac_addr_get(uint8_t *mac);
}


void initialize_id(void) {
  // create all the commands for reading and writing kv values 
  add_cmd("id",cmd_id,HTTPINTERFACE | CONSOLEINTERFACE);
}


/* #include "esp_system.h" */
/* String getMacAddress() { */
/* 	uint8_t baseMac[6]; */
/* 	// Get MAC address for WiFi station */
/* 	esp_read_mac(baseMac, ESP_MAC_WIFI_STA); */
/* 	char baseMacChr[18] = {0}; */
/* 	sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]); */
/* 	return String(baseMacChr); */
/* } */
