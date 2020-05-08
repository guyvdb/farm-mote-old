

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
esp_err_t read_storage_string_key(char *key, char *value, size_t len) {
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


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t write_storage_string_key(char *key, char *value) {
  esp_err_t err;
  nvs_handle_t handle;

  err = nvs_open("storage",NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  if( (err = nvs_set_str(handle, key, value)) == ESP_OK) {
    nvs_close(handle);
    return ESP_OK;
  } else {
    nvs_close(handle);
    return err;
  }
}
