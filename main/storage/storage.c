
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "storage.h"
#include "nvs.h"
#include "nvs_flash.h"


static const char* TAG = "STORAGE";








/* ------------------------------------------------------------------------
 * Initialise the non-volatile storate 
 * --------------------------------------------------------------------- */
void initialize_storage(void) {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK( nvs_flash_erase() );
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
}



/* ------------------------------------------------------------------------
 * Read a string value.
 * --------------------------------------------------------------------- */
static esp_err_t read_string_key(char *key, char *value, size_t len) {
  size_t stored_len;
  esp_err_t err;
  nvs_handle_t handle;

  err = nvs_open("storage",NVS_READONLY, &handle);
  if (err != ESP_OK) {
    return err;
  }

  if( (err = nvs_get_str(handle, key, NULL, &stored_len)) == ESP_OK) {
    if(stored_len <= len) {
      if ((err = nvs_get_str(handle,key, value,&len)) == ESP_OK) {
        nvs_close(handle);
        return ESP_OK;
      } else {
        nvs_close(handle);
        return err;
      }
    } else {
      nvs_close(handle);
      return ESP_ERR_INVALID_SIZE;
    }
  } else {
    nvs_close(handle);
    return err;
  }
}





// I (23263) WIFI: Failed to connect to SSID:HUAWEI-B618-7A5F, password:2H9RL35NY6R

/* ------------------------------------------------------------------------
 * Get the wifi ssid 
 * --------------------------------------------------------------------- */
esp_err_t get_wifi_ssid(char *buf, size_t len) {
  return  read_string_key("wifi.ssid",buf, len);
}

/* ------------------------------------------------------------------------
 * Set the wifi ssid 
 * --------------------------------------------------------------------- */
int set_wifi_ssid(char *buf) {
  return 0;
}

/* ------------------------------------------------------------------------
 * Get the wifi password
 * --------------------------------------------------------------------- */
esp_err_t get_wifi_password(char *buf, size_t len) {
  return  read_string_key("wifi.password",buf, len);
}


int set_wifi_password(char buf[64]) {
  return 0;
}

int get_wifi_max_retry() {
  return 10;
}

int set_wifi_max_retry(int value){
  return 0;
}

