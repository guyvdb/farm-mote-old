#include "command.h"
#include <console.h>

#include <stdio.h>
#include <sys/time.h>
#include <esp_err.h>

static int32_t get_timestamp() {
  return 10;
}


const char *command_to_string(uint8_t cmd) {
  switch(cmd) {
  case ACK:
    return ACK_STR;
  case NACK:
    return NACK_STR;
  case IDENTREQ:
    return IDENTREQ_STR;
  case IDENT:
    return IDENT_STR;
  case TIMEREQ:
    return TIMEREQ_STR;
  case TIMESET:
    return TIMESET_STR;
  case TIMEZONESET:
    return TIMEZONESET_STR;
  default:
    return UNKNOWN_STR;
  }
}

/* ------------------------------------------------------------------------
 * Generate a frame with the IDENT command. Push 4 bytes (uint32_t) onto 
 * the payload which represents the ID of this mote.
 * --------------------------------------------------------------------- */
frame_t *cmd_ident(uint32_t id){

  frame_t *frame = frame_create(IDENT,0, get_timestamp(), 4); // 4 byte long payload
  frame->cmd = IDENT;

  frame_args_begin(frame);
  if (!frame_args_put_uint32(frame,id)) {
    console_log_error("Failed to put mote id into frame arguments.");
  }
  frame_args_end(frame);
  
  return frame;
}


/* ------------------------------------------------------------------------
 * Generate a frame with the TIMEREQ command and no payload.
 * --------------------------------------------------------------------- */
frame_t *cmd_time_request() {
  frame_t *frame = frame_create(TIMEREQ, 0, get_timestamp(), 0); // no payload
  frame->cmd = TIMEREQ;
  return frame;
}

/*
time_t now;
char strftime_buf[64];
struct tm timeinfo;

time(&now);
// Set timezone to China Standard Time
setenv("TZ", "CST-8", 1);
tzset();

// Set timezone to South African Standard Time +2
setenv("TZ","SAST",2); 

localtime_r(&now, &timeinfo);
strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
*/



/* ------------------------------------------------------------------------
 * Extract a uint32_t from the payload of a frame with the command TIMESET.
 * Cast to signed and set the system time. Return 1 on success else 0 
 * --------------------------------------------------------------------- */
int cmd_time_set(frame_t *frame) {
  struct timeval t;
  uint32_t time;

  frame_args_begin(frame);
  if(!frame_args_get_uint32(frame, &time)) {
    console_log_error("Command TIMESET does not have a 4 byte payload.");
    frame_args_end(frame);
    return 0;
  }
  frame_args_end(frame);

  
  t.tv_sec = (int32_t)time;
  t.tv_usec = 0;
  
  if(settimeofday(&t,NULL) != 0) {
    console_log_error("Command TIMESET could not settimeofday");
    return 0;
  };
  return 1; 
}


/* ------------------------------------------------------------------------
 * Extract a uint8 and a length encoded string from the payload of the frame.
 * Set the timezone of the system. Return 1 on success else 0.
 * --------------------------------------------------------------------- */
int cmd_time_zone_set(frame_t *frame) {
  return 0;
}


/* ------------------------------------------------------------------------
 * Create a log frame. Specifiy the payload size for creating the frame.
 * The log type and log parameters are all put into the payload.
 * --------------------------------------------------------------------- */
frame_t *cmd_log(uint8_t payload_len) {
   return frame_create(LOG,0, get_timestamp(), payload_len); 
}
