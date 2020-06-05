#include "command.h"
#include "../console/log.h"

#include <stdio.h>
#include <sys/time.h>
#include <esp_err.h>

/* ------------------------------------------------------------------------
 * Generate a frame with the IDENT command. Push 4 bytes (uint32_t) onto 
 * the payload which represents the ID of this mote.
 * --------------------------------------------------------------------- */
frame_t *cmd_ident(uint32_t id){
  frame_t *frame = frame_create(IDENT, 4); // 4 byte long payload
  frame->cmd = IDENT;

  frame_args_begin(frame);
  if (!frame_put_arg_uint32(frame,id)) {
    log_error("Failed to put mote id into frame arguments.");
  }
  frame_args_end(frame);
  
  return frame;
}


/* ------------------------------------------------------------------------
 * Generate a frame with the TIMEREQ command and no payload.
 * --------------------------------------------------------------------- */
frame_t *cmd_time_request() {
  frame_t *frame = frame_create(TIMEREQ, 0); // no payload
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

  
  if(!frame_get_arg_uint32(frame, &time)) {
    log_error("Command TIMESET does not have a 4 byte payload.");
    return 0;
  }

  t.tv_sec = (int32_t)time;
  t.tv_usec = 0;
  
  if(settimeofday(&t,NULL) != 0) {
    log_error("Command TIMESET could not settimeofday");
    return 0;
  }

  printf("TIME SET\n");
  return 1; 
}


/* ------------------------------------------------------------------------
 * Extract a uint8 and a length encoded string from the payload of the frame.
 * Set the timezone of the system. Return 1 on success else 0.
 * --------------------------------------------------------------------- */
int cmd_time_zone_set(frame_t *frame) {
}
