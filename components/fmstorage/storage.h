#pragma once

#include "freertos/FreeRTOS.h"
#include "esp_err.h"


void initialize_storage(void);
esp_err_t read_storage_string_key(const char *key, char *value, size_t len);
esp_err_t write_storage_string_key(const char *key, char *value);
esp_err_t read_storage_uint16_key(const char *key, uint16_t *value);
esp_err_t write_storage_uint16_key(const char *key, uint16_t value);
esp_err_t read_storage_uint32_key(const char *key, uint32_t *value);
esp_err_t write_storage_uint32_key(const char *key, uint32_t value);

// Figure out the size of a blob in storage 
esp_err_t read_storage_blob_size(const char *key, size_t *len); 
esp_err_t read_storage_blob(const char *key, uint8_t *data, size_t len);
esp_err_t write_storage_blob(const char *key, uint8_t *data, size_t len);

