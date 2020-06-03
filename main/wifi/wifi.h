#pragma once

// This API is responsiable for maintaining connection to a wifi AP

void initialize_wifi(void); 
void deinitialize_wifi(void);
void create_wifi_task(void);
int wifi_valid(void);
