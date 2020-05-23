
#pragma once


#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


#define WIFI_INITIALIZED   BIT0
#define WIFI_CONNECTED     BIT1
#define WIFI_CONNECT_ERROR BIT2
#define WIFI_GOT_IP        BIT3







void initialize_events(void);
void finalize_events(void);



EventBits_t set_app_event_bits( const EventBits_t uxBitsToSet );
EventBits_t clear_app_event_bits(const EventBits_t uxBitsToClear );

EventGroupHandle_t get_app_event_group(void);

