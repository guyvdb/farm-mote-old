#pragma once

#include "frame.h"


// The command definitions 

#define ACK          0
#define NACK         1
/* RESERVED          2*/
/* RESERVED          3*/
#define IDENTREQ     4
#define IDENT        5
#define TIMEREQ      6
#define TIMESET      7
#define TIMEZONESET  8
#define LOG          9

/* RESERVED          27*/

#define UNKNOWN_STR             "UNKNOWN"
#define ACK_STR                 "ACK"
#define NACK_STR                "NACK"
#define IDENTREQ_STR            "IDENTREQ"
#define IDENT_STR               "IDENT"
#define TIMEREQ_STR             "TIMEREQ"
#define TIMESET_STR             "TIMESET"
#define TIMEZONESET_STR         "TIMEZONESET"
#define LOG_STR                 "LOG"



const char *command_to_string(uint8_t cmd);
uint32_t get_unix_timestamp(void);
frame_t *cmd_ident(uint32_t id);
frame_t *cmd_time_request();
int cmd_time_set(frame_t *frame);
int cmd_time_zone_set(frame_t *frame);





