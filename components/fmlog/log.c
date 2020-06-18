
#include "log.h"
#include <command.h>


//#include <stddef.h>
#include <stdint.h>


// Create a frame. Memory is allocated for the frame and len bytes is allocated for the payload
// cmd - the command to set on the frame
// ref - the refid of another frame
// timestamp - the timestamp to place on the frame 
// len - the length of the payload buffer allocated inside the frame
// return - a pointer to a new frame or null on error.
//frame_t *frame_create(uint8_t cmd, uint8_t ref, int32_t timestamp, size_t len);


/* ------------------------------------------------------------------------
 * Create a frame of a given logtype with a len size payload. Len excludes 
 * the 1 byte logtype identifier. The first payload argument is set to 
 * logtype but the argptr is not reset so additional arguments can be 
 * added without calling frame_args_begin() 
 * --------------------------------------------------------------------- */
static frame_t *log_create(uint8_t logtype, uint8_t len) {
  frame_t *frame = frame_create(LOG,0,get_unix_timestamp(),len + 1); // +1 for logtype
  frame_args_begin(frame);
  frame_args_put_uint8(frame, logtype);
  // do not call frame_args_end(). This is called by the caller of this func.
  return frame;  
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
frame_t *log_create_powerup(void) {
  frame_t *frame = log_create(LOGTYPE_POWERUP,0);
  frame_args_end(frame);
  return frame;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
frame_t *log_create_reboot(void) {
  frame_t *frame = log_create(LOGTYPE_REBOOT,0);
  frame_args_end(frame);
  return frame;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
frame_t *log_create_timeset(int32_t time) {
  frame_t *frame = log_create(LOGTYPE_TIMESET,4);
  frame_args_put_uint32(frame, (uint32_t)time);
  frame_args_end(frame);
  return frame;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
frame_t *log_create_relay_state_change(uint8_t relay, uint8_t newstate, int32_t time) {
  frame_t *frame = log_create(LOGTYPE_RELAY_STATE_CHANGE, 6);
  frame_args_put_uint8(frame, relay);
  frame_args_put_uint8(frame, newstate);
  frame_args_put_uint32(frame, (uint32_t)time);
  frame_args_end(frame);  
  return frame;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
frame_t *log_create_relay_timed_toggle_complete(uint8_t relay, int32_t start_time, int32_t end_time) {
  frame_t *frame = log_create(LOGTYPE_RELAY_TIMED_TOGGLE_COMPLETE, 9);
  frame_args_put_uint8(frame, relay);
  frame_args_put_uint32(frame, (uint32_t)start_time);
  frame_args_put_uint32(frame, (uint32_t)end_time);
  frame_args_end(frame);  
  return frame;
}
