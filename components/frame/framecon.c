#include "framecon.h"
#include <stdio.h>
#include <netinet/in.h>

#define READTIMEOUT  1000 * 10             // 10 milliseconds -- in microseconds
#define RXBUFLEN 256

static int wifi_interface_state = 0;
static int sock = -1;
static char[128] gateway_address = "";
static uint16_t gateway_port = 0;
static struct sockaddr_in dest_addr; 


static void log_conn_error() {
  printf("[ERROR] ");
  switch(errno) {
  case EAGAIN:
    printf("EAGAIN (try again)");
    break;
  case EBADF:
    printf("EBADF (bad file number)");
    break;
  case ECONNRESET:
    printf("ECONNRESET (connection reset by peer)");
    break;
  case EHOSTUNREACH :
    printf("EHOSTUNREACH (no route to host)");
    break;
  case ENFILE:
    printf("ENFILE (File table overflow)");
    break;
  default:
    printf("UNKNOWN");
    break;      
  }
  printf("\n");
}


/* ------------------------------------------------------------------------
 *
 * --------------------------------------------------------------------- */
static void socket_disconnect() {
  shutdown(socket,0);
  close(socket);
  sock = -1;
}

/* ------------------------------------------------------------------------
 * return 1 on success or 0 on failure 
 * --------------------------------------------------------------------- */
static int socket_connect() {

  int addr_family;
  int ip_protocol;

  // read timeout option
  struct timeval tv; 
  tv.tv_sec = 0; 
  tv.tv_usec = READTIMEOUT; 
  

  // close the socket if it is open 
  if(sock != -1) {
    socket_disconnect();
  }

  // set up the sockaddr_in structure 
  dest_addr.sin_addr.s_addr = inet_addr(host);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(port);

  // TODO why are these stored in variables?
  addr_family = AF_INET;
  ip_protocol = IPPROTO_IP;

  // create the socket 
  sock = socket(addr_family, SOCKET_STREAM, ip_protocol);
  if(sock < 0) {
    log_conn_error();
    return 0;
  }

  // set read timeout
  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0)  {
    log_conn_error();
    socket_disconnect();
    return 0;
  }

  int cerr = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (cerr != 0) {
    log_conn_error();
    socket_disconnect();
    return 0;
  }

  
  return 1;  
}





/* ------------------------------------------------------------------------
 * The state has changed. Either gateway address/port or interface
 * --------------------------------------------------------------------- */
static void state_changed() {
  if(wifi_interface_state && gateway_address[0] != 0x0 && gateway_port != 0) {
    socket_connect();
  } else {
    socket_disconnect();
  }
}

/* ------------------------------------------------------------------------
 *
 * --------------------------------------------------------------------- */
int framecon_set_gateway_address(const char *address) {
  if(strlen(address) + 1 < sizeof(gateway_address)) {
    strcpy(gateway_address, address);
    state_changed();
    return 1;
  } else {
    printf("Out of memory cannot set gateway address.\n");
    return 0;
  }
}

/* ------------------------------------------------------------------------
 *
 * --------------------------------------------------------------------- */
int framecon_set_gateway_port(uint16_t port) {
  if (port != 0) {
    gateway_port = port;
    state_changed();
    return 1;
  } else {
    printf("Will not set gateway port to 0\n");
    return 0;
  }
}

/* ------------------------------------------------------------------------
 *
 * --------------------------------------------------------------------- */
void framecon_wifi_interface_state_change (int state) {
  wifi_interface_state = state;
  state_changed();
}

/* ------------------------------------------------------------------------
 *
 * --------------------------------------------------------------------- */
int framecon_ready() {
  if(socket > -1) {
    return 1;
  } else {
    return 0;
  }
}


/* ------------------------------------------------------------------------
 * read a frame from the framecon. If framecon does read a frame
 * it will allocate memory for it. It is the callers responsibility
 * to call frame_free() at some point.
 * return - a frame_t pointer on success or null on failure.
 * --------------------------------------------------------------------- */
frame_t *framecon_read() {
  uint8_t buf[RXBUFLEN];
  int nbytes = read(sock, buf, RXBUFLEN - 1);

  if(nbytes < 0) {
    log_conn_error();
    return 0x0;
  } else if (nbytes == 0) {
    log_conn_error();
    socket_disconnect();
    return 0x0;
  } else {
    framebuf_write(buf, nbytes);
    return framebuf_deframe();
  }
}



/* ------------------------------------------------------------------------
 * write a frame
 * return - 1 on success or 0 on failure 
 * --------------------------------------------------------------------- */
int framecon_write(frame_t *frame) {
  uint8_t buf[RXBUFLEN];
  int encoded;
  int len = frame_encoded_len(frame);
  
  if(len < sizeof(buf)) {
    encoded = frame_encode(frame, buf, sizeof(buf));
    if (encoded == 0) {
      printf("failed to encode\n");
      return 0;
    } else {
      int nbytes = write(sock, buf, encoded);
      if(nbytes == -1) {
        log_conn_error();
        return 0;
      } else if(nbytes == 0) {
        log_conn_error();
        return 0;
      } else if(nbytes != encoded) {
        printf("Todo: transmit more data here\n");
        return 0;
      } else {
        return 1;
      }
    }
  } else {
    printf("Out of memory\n");
    return 0;
  }
}


