#pragma once



// This API is responsiable for maintaining connection to a wifi AP

void initialize_wifi(void);
void deinitialize_wifi(void);

void wifi_ap_initialize(void);
void wifi_ap_connect(void);
void wifi_ap_disconnect(void);


void create_wifi_task(void);

