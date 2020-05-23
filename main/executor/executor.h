#pragma once


//#include "freertos/FreeRTOS.h"
//#include "freertos/event_groups.h"



// The executor is communicating with the server


void executor_task( void *pvParameters );
void initialize_executor(void); //EventGroupHandle_t events);
void finalize_executor(void);
