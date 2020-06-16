#pragma once

#include <frame.h>





#define LOGTYPE_POWERUP                           0
#define LOGTYPE_REBOOT                            1
#define LOGTYPE_TIMESET                           2
#define LOGTYPE_SWITCH_STATE_CHANGE               3
#define LOGTYPE_SWITCH_TIMED_TOGGLE_COMPLETE      4







// This file represents the events that can be logged to the server 
frame_t *log_create_powerup(void);
frame_t *log_create_reboot(void);
frame_t *log_create_timeset(int32_t time);
frame_t *log_create_switch_state_change(uint32_t switch_num, uint8_t newstate, int32_t time);
frame_t *log_create_switch_timed_toggle_complete(uint32_t switch_num, int32_t start_time, int32_t end_time, uint8_t start_state, uint8_t end_state);
