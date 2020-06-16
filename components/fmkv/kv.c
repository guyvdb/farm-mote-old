#include "kv.h"
#include <stdio.h>
#include <string.h>



#include <storage.h>
//#include <command.h>







/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_kv(void) {
  // create all the commands for reading and writing kv values 

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

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t get_gateway_address(char *buf, size_t len) {
  return read_storage_string_key("gateway.address",buf,len);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t set_gateway_address(char *value) {
  return write_storage_string_key("gateway.address",value);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t get_gateway_port(uint16_t *port) {
  return read_storage_uint16_key("gateway.port", port);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t set_gateway_port(uint16_t port) {
  return write_storage_uint16_key("gateway.port", port);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t get_id(uint32_t *value) {
  return read_storage_uint32_key("id", value);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t set_id(uint32_t value) {
  return write_storage_uint32_key("id", value);
}



/* ------------------------------------------------------------------------
 * Get the count of switches stored int he kv store. Switches are stored
 * as an array of uint8_t 
 * --------------------------------------------------------------------- */
size_t get_relay_pin_count() {
  size_t count;
  esp_err_t err;

  err = read_storage_blob_size("relay.pins",&count);
  if(err != ESP_OK) {
    printf("ERROR failed to get switch count from kv\n");
    return 0;
  }
  return count;
}

/* ------------------------------------------------------------------------
 * Get the switches. The value buffer should be get_switch_count() bytes long.
 * value - a pointer to set the result to
 * len - the amount of memory pointed to by value  
 * --------------------------------------------------------------------- */
esp_err_t get_relay_pins(uint8_t *value, size_t len) {
  return read_storage_blob("relay.pins",value, len);
}

/* ------------------------------------------------------------------------
 * Set the switches
 * value - the data to set
 * len - the number of switches 
 * --------------------------------------------------------------------- */
esp_err_t set_relay_pins(uint8_t *value, size_t len) {
  return write_storage_blob("relay.pins",value, len);
}

/*
// Figure out the size of a blob in storage 
esp_err_t read_storage_blob_size(const char *key, size_t *len); 
esp_err_t read_storage_blob(const char *key, uint8_t *data, size_t len);
esp_err_t write_storage_blob(const char *key, uint8_t *data, size_t len);

*/
