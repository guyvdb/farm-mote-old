#pragma once
#include <esp_err.h>

void initialize_kv(void);

esp_err_t get_wifi_ssid(char *buf, size_t len);
esp_err_t set_wifi_ssid(char *value);
esp_err_t get_wifi_password(char *buf, size_t len);
esp_err_t set_wifi_password(char *value);
esp_err_t get_gateway_address(char *buf, size_t len);
esp_err_t set_gateway_address(char *value);
esp_err_t get_gateway_port(uint16_t *port);
esp_err_t set_gateway_port(uint16_t port);
