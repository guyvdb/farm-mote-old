#pragma once

#include "frame.h"


// The command definitions 

#define ACK          0
#define NACK         1
/* RESERVED          2*/
/* RESERVED          3*/
#define IDENT        4
#define TIMEREQ      5
#define TIMESET      6
#define TIMEZONESET  7

/* RESERVED          27*/





frame_t *cmd_ident(uint32_t id);
frame_t *cmd_time_request();
int cmd_time_set(frame_t *frame);
int cmd_time_zone_set(frame_t *frame);


