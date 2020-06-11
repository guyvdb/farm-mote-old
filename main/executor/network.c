#include "network.h"
#include "framebuf.h"
#include <lwip/sockets.h>
#include <esp_err.h>
#include "../kv/kv.h"
#include "../console/log.h"

#define MAXCONNDELAY 1000 * 30             // 30 seconds -- in milliseconds
#define READTIMEOUT  1000 * 10             // 10 milliseconds -- in microseconds
#define RXBUFLEN  512

static int createdflag = 0;                // have we created a socket
static int connectedflag = 0;              // have we connected the socket
static int sock = -1;                      // the socket handle 
static int conndelay = 0;                  // time to delay the connection in seconds
static struct sockaddr_in dest_addr;       // the gateway address
static uint8_t rxbuf[RXBUFLEN];            // the read buffer 


/* ------------------------------------------------------------------------
 * create the socket. return the socket handle 
 * --------------------------------------------------------------------- */
int socket_create() {
  esp_err_t err;
  int addr_family;
  int ip_protocol;
  char host[128];
  uint16_t port;

  // read timeout option
  struct timeval tv; 
  tv.tv_sec = 0; 
  tv.tv_usec = READTIMEOUT; 

  
  if(createdflag == 0) {

    //printf("createdflag is 0. creating a new socket.\n");
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

    // set read timeout option 
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0)  {
      log_std_error(errno, "Networking failed to set socket options.");
      return -1;
    }

    // Success 
    createdflag = 1;

    //printf("createdflag set to 1. created new socket. sock=%d\n", sock);
    return sock;
    
  } else {
    // already created - return the existing socket
    //printf("createdflag is 1. returning existing socket.\n");
    return sock;
  }
}


/* ------------------------------------------------------------------------
 * connect the socket
 * --------------------------------------------------------------------- */
int socket_connect() {
  if(connectedflag == 0) {
    int cerr = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (cerr != 0) {
      log_std_error(errno,"Networking failed to connect.");
      return 0;
    } else {
      log_info("Networking connected to gateway.");
      connectedflag = 1;
      return 1;
    }
  } else {
    return 1;
  }
}

/* ------------------------------------------------------------------------
 * disconnect the socket
 * --------------------------------------------------------------------- */
void socket_disconnect() {
  //printf("disconnecting socket\n");
  shutdown(sock, 0);
  close(sock);
  connectedflag = 0;
  createdflag = 0;
  sock = -1;
}


/* ------------------------------------------------------------------------
 * check if the socket is valid
 * --------------------------------------------------------------------- */
int socket_valid() {
  if (connectedflag == 0 || createdflag == 0) {
    return 0;
  } else {
    return 1;
  }
}


// check if the socket is created
int socket_created() {
  return createdflag;
}

// check if the socket is connected
int socket_connected() {
  return connectedflag;
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

/* ------------------------------------------------------------------------
 * read a frame of the socket. a frame is created on success. it is the 
 * responsability of the caller to free the memory. null is returned if a 
 * frame is not available.
 * --------------------------------------------------------------------- */
frame_t *socket_read_frame() {
  int nbytes = read(sock, rxbuf, RXBUFLEN - 1);
  if (nbytes < 0) {
    if (errno != EAGAIN) {
      log_std_error(errno,"Networing read error.");
      socket_disconnect();
    } else if (errno == EAGAIN) {
      //printf("read timeout\n");
    }
    return 0x0;  
  } else if (nbytes == 0) {
    log_std_error(errno, "Network is disconnected.");
    socket_disconnect();
    return 0x0;
  } else {
    framebuf_write(rxbuf, nbytes);
    return framebuf_deframe();
  }
}

/*
static void printbuf(uint8_t *data, size_t len) {
  printf("[");
  for(int i=0;i<len;i++){
    if(i+1 == len) {
      printf("%d",data[i]);
    } else {
      printf("%d ",data[i]);
    }
  }
  printf("]\n");
}
*/

/* ------------------------------------------------------------------------
 * write a frame onto the socket. 1 = success, 0 = error 
 * --------------------------------------------------------------------- */
int socket_write_frame(frame_t *frame) {
  uint8_t buf[256];
  int encoded;
  int len = frame_encoded_len(frame);

  //printf("socket_write_frame: sock=%d\n", sock);
  

  if (len < sizeof(buf)) {
    encoded = frame_encode(frame, buf,sizeof(buf));
    if (encoded == 0) {
      // failed to encode
      return 0;
    } else {

      //printf("encoded=%d\n",encoded);
      //printbuf(buf, encoded);

      
      int nbytes = write(sock, buf, encoded);

      //printf("nbytes=%d\n",nbytes);

      if (nbytes == -1) {
        log_std_error(errno, "nbytes=-1");
        return 0;
      } else if(nbytes != encoded) {
        printf("TODO transmit unitl all data sent\n");
        return 0;
      } else {
        return 1;
      }
    }
  } else {
    printf("Out of memory error\n");
    return 0;
  }
}
