#pragma once

#include "freertos/FreeRTOS.h"

// This file deals with bytes and the conversion of values to/from big endian network byte order




// In the decode functions the data pointer must have enough space for the operation 
uint8_t bytes_uint8_decode(uint8_t *data);
uint16_t bytes_uint16_decode(uint8_t *data);
uint32_t bytes_uint32_decode(uint8_t *data);

// This allocates memory for result. The caller must free the memory.
char *bytes_string_decode(uint8_t *data);

// In the encode functions the result pointer must have enough space for the operation
void bytes_uint8_encode(uint8_t value, uint8_t *result);
void bytes_uint16_encode(uint16_t value, uint8_t *result);
void bytes_uint32_encode(uint32_t value, uint8_t *result);


void bytes_string_encode(const char* value, uint8_t *result);


//uint32_t myInt1 = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
