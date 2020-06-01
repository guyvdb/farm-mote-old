#pragma once

/*

Common Error Codes

ECONNREFUSED -- Connection refused
EADDRINUSE
ECONNABORTED
ENETUNREACH
ENETDOWN
ETIMEOUT
EHOSTDOWN
EHOSTUNREACH
EINPROGRESS
EALREADY
EDESTADDRREQ
EPROTONOSUPPORT




 */


// create the socket
int socket_create();

// destroy the socket
void socket_destroy();

// connect the socket
int socket_connect();

// disconnect the socket
void socket_disconnect();

// check if the socket is valid
int socket_valid();

// read the socket
int socket_read();

// write the socket 
int socket_write();

