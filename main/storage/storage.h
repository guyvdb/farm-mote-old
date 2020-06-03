#pragma once

#include "freertos/FreeRTOS.h"
#include "esp_err.h"


void initialize_storage(void);
esp_err_t read_storage_string_key(char *key, char *value, size_t len);
esp_err_t write_storage_string_key(char *key, char *value);
esp_err_t read_storage_uint16_key(char *key, uint16_t *value);
esp_err_t write_storage_uint16_key(char *key, uint16_t value);
esp_err_t read_storage_uint32_key(char *key, uint32_t *value);
esp_err_t write_storage_uint32_key(char *key, uint32_t value);
