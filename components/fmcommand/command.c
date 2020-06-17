#include "command.h"
#include <console.h>

#include <stdio.h>
#include <sys/time.h>
#include <esp_err.h>




/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
uint32_t get_unix_timestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  return (uint32_t)tv.tv_sec;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
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
  case LOG:
    return LOG_STR;
  default:
    return UNKNOWN_STR;
  }
}

/* ------------------------------------------------------------------------
 * Generate a frame with the IDENT command. Push 4 bytes (uint32_t) onto 
 * the payload which represents the ID of this mote.
 * --------------------------------------------------------------------- */
frame_t *cmd_ident(uint32_t id){

  frame_t *frame = frame_create(IDENT,0, get_unix_timestamp(), 4); // 4 byte long payload
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
  frame_t *frame = frame_create(TIMEREQ, 0, get_unix_timestamp(), 0); // no payload
  frame->cmd = TIMEREQ;
  return frame;
}

/* ------------------------------------------------------------------------
 * Extract a uint32_t from the payload of a frame with the command TIMESET.
 * Cast to signed and set the system time. Extract a string from the payload
 * use this to set the timezone. 
 * Return 1 on success else 0 
 * --------------------------------------------------------------------- */
int cmd_time_set(frame_t *frame) {
  struct timeval t;
  uint32_t time;
  char tz[16];

  
  frame_args_begin(frame);

  // Extract the unix timestamp 
  if(!frame_args_get_uint32(frame, &time)) {
    console_log_error("Command TIMESET does not have a 4 byte unix timestamp.");
    frame_args_end(frame);
    return 0;
  }

  // Extract the timezone string length 
  size_t slen;
  if(!frame_args_get_string_len(frame, &slen)) {
    console_log_error("Command TIMESET does not have a string timezone.");
    frame_args_end(frame);
    return 0;
  }

  // Extract the timezone  
  if(slen < 16) {
    if(!frame_args_get_string(frame, tz, slen)) {
      console_log_error("Command TIMSET failed to get timezone from args.");
      frame_args_end(frame);
      return 0;
    }                                                 
  } else {
    console_log_error("Command TIMESET buffer too small to get timezone.");
    frame_args_end(frame);
    return 0;
  }

  // end the frame args
  frame_args_end(frame);

  // set the time 
  t.tv_sec = (int32_t)time;
  t.tv_usec = 0;
  if(settimeofday(&t,NULL) != 0) {
    console_log_error("Command TIMESET could not settimeofday");
    return 0;
  };

  // set the timezone 
  setenv("TZ", tz, 1);
  tzset();


  // complete
  return 1; 
}


/* ------------------------------------------------------------------------
 * Extract a uint8 and a length encoded string from the payload of the frame.
 * Set the timezone of the system. Return 1 on success else 0.
 * --------------------------------------------------------------------- */
int cmd_time_zone_set(frame_t *frame) {
  return 0;
}


