

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
esp_err_t read_storage_string_key(const char *key, char *value, size_t len) {
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
esp_err_t write_storage_string_key(const char *key, char *value) {
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

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t read_storage_uint16_key(const char *key, uint16_t *value) {
  esp_err_t err;
  nvs_handle_t handle;

  err = nvs_open("storage",NVS_READONLY, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_get_u16(handle, key, value);
  nvs_close(handle);
  return err;
  
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t write_storage_uint16_key(const char *key, uint16_t value) {
  esp_err_t err;
  nvs_handle_t handle;

  err = nvs_open("storage",NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_u16(handle, key, value);
  nvs_close(handle);
  return err;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t read_storage_uint32_key(const char *key, uint32_t *value) {
  esp_err_t err;
  nvs_handle_t handle;

  err = nvs_open("storage",NVS_READONLY, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_get_u32(handle, key, value);
  nvs_close(handle);
  return err;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t write_storage_uint32_key(const char *key, uint32_t value){
  esp_err_t err;
  nvs_handle_t handle;

  err = nvs_open("storage",NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_u32(handle, key, value);
  nvs_close(handle);
  return err;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t read_storage_blob_size(const char *key, size_t *len) {
  esp_err_t err;
  nvs_handle_t handle;

  err = nvs_open("storage", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }


  *len = 0; // set result to 0 if there is no key

  err = nvs_get_blob(handle, key, 0x0, len);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
    nvs_close(handle);
    return err;
  }

  nvs_close(handle);
  return ESP_OK;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t read_storage_blob(const char *key, uint8_t *data, size_t len) {
  esp_err_t err;
  nvs_handle_t handle;
  size_t size;

  err = nvs_open("storage", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  // get required mem size
  err = nvs_get_blob(handle, key, 0x0, &size);
  if (err != ESP_OK) { 
    nvs_close(handle);
    return err;
  }

  // buffer is too small 
  if(len < size) {
    nvs_close(handle);
    return ESP_ERR_NO_MEM;
  }

  // read the value
  err = nvs_get_blob(handle, key, data, len);
  if (err != ESP_OK) {
    nvs_close(handle);
    return err;
  }

  nvs_close(handle);
  return ESP_OK;  
}



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t write_storage_blob(const char *key, uint8_t *data, size_t len) {
  esp_err_t err;
  nvs_handle_t handle;


  err = nvs_open("storage", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_blob(handle,key,data,len);
  if (err != ESP_OK) {
    nvs_close(handle);
    return err;
  }


  nvs_close(handle);
  return ESP_OK;  
}
