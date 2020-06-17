#pragma once

#include <frame.h>





#define LOGTYPE_POWERUP                           0
#define LOGTYPE_REBOOT                            1
#define LOGTYPE_TIMESET                           2
#define LOGTYPE_RELAY_STATE_CHANGE                3
#define LOGTYPE_RELAY_TIMED_TOGGLE_COMPLETE       4







// This file represents the events that can be logged to the server 
frame_t *log_create_powerup(void);
frame_t *log_create_reboot(void);
frame_t *log_create_timeset(int32_t time);
frame_t *log_create_relay_state_change(uint8_t relay, uint8_t newstate, int32_t time);
frame_t *log_create_relay_timed_toggle_complete(uint8_t relay, int32_t start_time, int32_t end_time); 
