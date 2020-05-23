#pragma once

//#include "freertos/FreeRTOS.h"
//#include "freertos/event_groups.h"


// This API is responsiable for maintaining connection to a wifi AP

void initialize_wifi(void); //EventGroupHandle_t events);
void deinitialize_wifi(void);

void wifi_ap_initialize(void);
void wifi_ap_connect(void);
void wifi_ap_disconnect(void);


void create_wifi_task(void);

