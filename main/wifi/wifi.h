#pragma once

// This API is responsiable for maintaining connection to a wifi AP

#define WIFI_INITIALIZED   BIT0
#define WIFI_CONNECTED     BIT1
#define WIFI_CONNECT_ERROR BIT2
#define WIFI_GOT_IP        BIT3


void initialize_wifi(void); 
void deinitialize_wifi(void);
void create_wifi_task(void);
void wifi_wait_for_interface(void);
int wifi_valid(void);
