
#pragma once


#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


#define WIFI_INITIALIZED   BIT0
#define WIFI_CONNECTED     BIT1
#define WIFI_CONNECT_ERROR BIT2
#define WIFI_GOT_IP        BIT3









#define EVENT_NETWORK (1 << 0)
#define EVENT_SENSOR (1 << 1)
#define EVENT_ACTUATOR ( 1 << 2)

#define cNOOP         0

#define cCONNECTED    1
#define cDISCONNECTED 2


#define cID           3
#define cTIME         4



extern EventGroupHandle_t app_event_group;


void initialize_events(void);
void finalize_events(void);



struct event {
  BaseType_t group;
} event_t;



// commands that can be sent as events on the queue









//EventBits_t set_app_event_bits( const EventBits_t uxBitsToSet );
//EventBits_t clear_app_event_bits(const EventBits_t uxBitsToClear );

//EventGroupHandle_t get_app_event_group(void);

