#pragma once

#include "frame.h"


// create the socket
int socket_create();

// destroy the socket
//void socket_destroy();

// connect the socket
int socket_connect();

// disconnect the socket
void socket_disconnect();

// create and connect the socket
//int socket_create_and_connect();

// disconnect from the socket and destroy it.
//void socket_disconnect_and_destroy();

// check if the socket is valid
int socket_valid();

// check if the socket is created
int socket_created();

// check if the socket is connected
int socket_connected();


// read a frame 
frame_t *socket_read_frame();

// write a frame 
int socket_write_frame(frame_t *frame);

// read the socket
int socket_read();

// write the socket 
int socket_write();

