
#pragma once


#include "freertos/FreeRTOS.h"
#include "esp_err.h"


void initialize_storage(void);



// Wifi Related Storage





esp_err_t  get_wifi_ssid(char *buf, size_t len);
int set_wifi_ssid(char *buf);
esp_err_t get_wifi_password(char *buf, size_t len);
int set_wifi_password(char *buf);
int get_wifi_max_retry();
int set_wifi_max_retry(int value);







/* // These defines need to come from the store  */
/* #define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY */
/* #define EXAMPLE_ESP_WIFI_SSID      "HUAWEI-B618-7A5F" */
/* #define EXAMPLE_ESP_WIFI_PASS      "2H9RL35NY6R"  */
