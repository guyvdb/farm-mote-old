#include "esp_err.h"
#include "kv.h"
#include "../storage/storage.h"
#include "../command/command.h"
#include <stdio.h>


#define MAX_STRING_VALUE_LEN 255


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_wifi_ssid(char *argv[], int argc) {
  char value[MAX_STRING_VALUE_LEN];
  esp_err_t err;
  
  switch(argc) {
  case 1  :
    err = get_wifi_ssid(value,sizeof(value));
    if (err == ESP_OK) {
      printf("ssid: %s\n",value);
    } else {
      printf("[ERROR] %s\n", esp_err_to_name(err));
    }
    break; 
  case 2  :
    err = set_wifi_ssid(argv[1]);
    if (err == ESP_OK) {
      printf("ssid set\n");
    } else {
      printf("[ERROR] %s\n", esp_err_to_name(err));
    }    
    break; 
  default :
    printf("[ERROR] too many arguments. wifi-ssid | wifi-ssid <value>\n");
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_wifi_password(char *argv[], int argc) {
  char value[MAX_STRING_VALUE_LEN];
  esp_err_t err;
  

  switch(argc) {
  case 1  :
    err = get_wifi_password(value,sizeof(value));
    if (err == ESP_OK) {
      printf("password: %s\n",value);
    } else {
      printf("[ERROR] %s\n", esp_err_to_name(err));
    }
    break; 
  case 2  :
    err = set_wifi_password(argv[1]);
    if (err == ESP_OK) {
      printf("password set\n");
    } else {
      printf("[ERROR] %s\n", esp_err_to_name(err));
    }    
    break; 
  default :
    printf("[ERROR] too many arguments. wifi-password | wifi-password <value>\n");
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_kv(void) {
  // create all the commands for reading and writing kv values 
  add_cmd("wifi-ssid",cmd_wifi_ssid,HTTPINTERFACE | CONSOLEINTERFACE);
  add_cmd("wifi-password",cmd_wifi_password, HTTPINTERFACE | CONSOLEINTERFACE);
}




/* ------------------------------------------------------------------------
 * Get the wifi ssid 
 * --------------------------------------------------------------------- */
esp_err_t get_wifi_ssid(char *buf, size_t len) {
  return  read_storage_string_key("wifi.ssid",buf, len);
}

/* ------------------------------------------------------------------------
 * Set the wifi ssid 
 * --------------------------------------------------------------------- */
esp_err_t set_wifi_ssid(char *value) {
  return write_storage_string_key("wifi.ssid",value);
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t get_wifi_password(char *buf, size_t len) {
  return read_storage_string_key("wifi.password",buf,len);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t set_wifi_password(char *value){
  return write_storage_string_key("wifi.password",value);
}


