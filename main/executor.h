#pragma once

#include <frame.h>

void executor_task( void *pvParameters );
void initialize_executor(void); //EventGroupHandle_t events);
void finalize_executor(void);
void transmit(frame_t* frame);
