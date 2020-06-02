#include "network.h"

#include <lwip/sockets.h>
#include <esp_err.h>

#include "../kv/kv.h"
#include "../console/log.h"


#define MAXCONNDELAYSECONDS 10


// the socket descriptor
static int createdflag = 0; // have we created a socket 
static int sock = -1;
static int conndelay = 0; // time to delay the connection in seconds
static struct sockaddr_in dest_addr;



/* ------------------------------------------------------------------------
 * create the socket. return the socket handle 
 * --------------------------------------------------------------------- */
int socket_create() {
  esp_err_t err;
  int addr_family;
  int ip_protocol;
  char host[128];
  uint16_t port;

  struct timeval tv;

  tv.tv_sec = 1; // the socket read timeout in seconds
  tv.tv_usec = 0; // the socket read timeout in mircoseconds
   
  if(createdflag == 0) {
    // Get the gateway address 
    err = get_gateway_address(host, sizeof(host));
    if (err != ESP_OK) {
      log_error("Networking failed to get gateway address. %s.",esp_err_to_name(err));
      return -1;
    }

    // Get the gateway port
    err = get_gateway_port(&port);
    if (err != ESP_OK) {
      log_error("Networking failed to get gateway port. %s.",esp_err_to_name(err));
      return -1;
    }


    dest_addr.sin_addr.s_addr = inet_addr(host);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    // TODO do I need these variables or can AF_INET and IPPROTO_IP be used directly?
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    // Create the socket 
    sock =  socket(addr_family, SOCK_STREAM, ip_protocol);

    // was there an error in creating the socket 
    if (sock < 0) {
      log_std_error(errno, "Networking failed to create socket.");
      return sock;
    }

    // set the socket options
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0)  {
      log_std_error(errno, "Networking failed to set socket options.");
      return -1;
    }

    // Success 
    createdflag = 1;
    return sock;
    
  } else {
    // already created - return the existing socket 
    return sock;
  }
}


/* ------------------------------------------------------------------------
 * destroy the socket
 * --------------------------------------------------------------------- */
void socket_destroy() {
  sock = -1;
  createdflag = 0;
}

/* ------------------------------------------------------------------------
 * connect the socket
 * --------------------------------------------------------------------- */
int socket_connect(int sock) {
  int cerr = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (cerr != 0) {
    log_std_error(errno,"Networking failed to connect.");
    return 0;
  } else {
    printf("Connected.\n");
    return 1;
  }
}

/* ------------------------------------------------------------------------
 * disconnect the socket
 * --------------------------------------------------------------------- */
void socket_disconnect(int sock) {
  shutdown(sock, 0);
  close(sock);
}

/* ------------------------------------------------------------------------
 * check if the socket is valid
 * --------------------------------------------------------------------- */
int socket_valid() {
  return 0;
}

/* ------------------------------------------------------------------------
 * read the socket
 * --------------------------------------------------------------------- */
int socket_read(){
  return 0;
}

/* ------------------------------------------------------------------------
 * write the socket 
 * --------------------------------------------------------------------- */
int socket_write() {
  return 0;
}


