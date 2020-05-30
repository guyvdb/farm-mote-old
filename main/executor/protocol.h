#pragma once

#include "freertos/FreeRTOS.h"

#define PROTOBUFLEN 32
#define ESCAPE 0x1B
#define SFLAG 0x2
#define EFLAG 0x3




void protobuf_reset();
int protobuf_put(uint8_t data);
int protobuf_write(uint8_t *data, size_t len);
int protobuf_size();
int protobuf_frame_size();
int protobuf_deframe(uint8_t *data, size_t len);




  


void protobuf_print();








