#pragma once

#include "frame.h"


// The command definitions 

#define ACK          0
#define NACK         1
/* RESERVED          2*/
/* RESERVED          3*/
#define IDENT        4
#define TIMEREQ      5
#define TIMERESP     6
/* RESERVED          27*/





frame_t *cmd_ident(uint32_t id);




