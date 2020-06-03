#include "command.h"

#include "../console/log.h"




frame_t *cmd_ident(uint32_t id){
  frame_t *frame = frame_create(IDENT, 4); // 4 byte long payload
  frame->cmd = IDENT;
  if (!frame_put_arg_uint32(frame,id)) {
    log_error("Failed to put mote id into frame arguments.");
  }

  return frame;
}
