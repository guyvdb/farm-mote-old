
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

esp_err_t get_id(uint32_t *value);
esp_err_t set_id(uint32_t value);


// Get the count of switches stored int he kv store 
size_t get_relay_pin_count();

// Get the switches
// value - a pointer to set the result to
// len - the amount of memory pointed to by value 
esp_err_t get_relay_pins(uint8_t *value, size_t len);

// Set the switches
// value - the data to set
// len - the number of switches
esp_err_t set_relay_pins(uint8_t *value, size_t len);

