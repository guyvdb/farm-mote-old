
#pragma once


#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


#define WIFI_INITIALIZED   BIT0
#define WIFI_CONNECTED     BIT1
#define WIFI_CONNECT_ERROR BIT2
#define WIFI_GOT_IP        BIT3


extern EventGroupHandle_t app_event_group;


void initialize_events(void);
void finalize_events(void);



struct event {
  BaseType_t group;
} event_t;
