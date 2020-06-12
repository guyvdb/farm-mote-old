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


