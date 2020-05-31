#pragma once



// A command goes into a frame. The frame is sent from the server or from the client. The receiver of the command
// executes it, if appropriate. The execution might result in an ACK or NACK to the other side.
//
// Commands have a common section and then a specific section.

struct command {
};


typedef struct command command_t;


