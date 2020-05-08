#pragma once

#include "freertos/FreeRTOS.h"
#include "esp_err.h"


void initialize_storage(void);
esp_err_t read_storage_string_key(char *key, char *value, size_t len);
esp_err_t write_storage_string_key(char *key, char *value);


