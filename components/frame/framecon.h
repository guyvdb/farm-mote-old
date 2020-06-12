#pragma once

#include <stdint.h>
#include "frame.h"


// framecon is a client connection. it manages the state of the connection
// and can read and write frames to/from the wire.




// register a function that can get the gateway address
// register a function that can get the gateway port



// Set the gateway address for the frameconn.
// address - the ipv4 dot address string (e.g. 192.168.0.x)
// return - 1 on success or 0 on failure
int framecon_set_gateway_address(const char *address);

// Set the port of the gateway
// port - the port number
// return - 1 on success or 0 on failure
int framecon_set_gateway_port(uint16_t port);


// Call this function to notify the framecon of a
// change in the state of the wifi interface
// state - the state of the interface. 1=up, 0=down
void framecon_wifi_interface_state_change (int state);


int framecon_reconnect(void);

// Is the framecon read to perform frame exchance
// return - 1 read or 0 not ready 
int framecon_ready(void);

// read a frame from the framecon. If framecon does read a frame
// it will allocate memory for it. It is the callers responsibility
// to call frame_free() at some point.
// return - a frame_t pointer on success or null on failure.
frame_t *framecon_read();

// write a frame
// return - 1 on success or 0 on failure 
int framecon_write(frame_t *frame);
