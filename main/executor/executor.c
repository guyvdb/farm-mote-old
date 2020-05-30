#include "executor.h"
#include "protocol.h"

#include <string.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_err.h"

#include "../kv/kv.h"
#include "../event/event.h"
#include "../command/command.h"


#define MAX_FAILED_SOCKETS_TO_REBOOT 10
#define MAX_CONNECTION_DELAY 4000
#define RX_BUF_SIZE 256


//#define HOST_IP_ADDR "192.168.8.100"
//#define PORT 3000
#define MAX_ERROR 300

static char host[128];
static uint16_t port;

static const char *payload = "PING\n";
static struct sockaddr_in dest_addr;
static int running = 0;

static int connection_delay;
static int failed_socket_count;


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_reboot(char *argv[], int argc, printfunc print){
  print("Will reboot in 1 second\n");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  esp_restart();
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_executor() {
  running = 1;
  add_cmd("reboot",cmd_reboot, CONSOLEINTERFACE);  
  xTaskCreate(executor_task, "executor_task", 4096, NULL, 5, NULL);
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void finalize_executor(void) {

}



/* ------------------------------------------------------------------------
 *  Create and return the open socket 
 * --------------------------------------------------------------------- */
static int socket_connect() {

  esp_err_t err;
  int addr_family;
  int ip_protocol;
  char addr_str[128];


  struct timeval tv;

  
  tv.tv_sec = 1;
  tv.tv_usec = 0; // 

  
  // gateway address
  err = get_gateway_address(host, sizeof(host));
  if (err != ESP_OK) {
     printf("[ERROR] %s\n", esp_err_to_name(err));
     return -1;
  }

  // gateway port 
  err = get_gateway_port(&port);
  if (err != ESP_OK) {
    // printf("[ERROR] %s\n", esp_err_to_name(err));
     return -1;
  }

  
  dest_addr.sin_addr.s_addr = inet_addr(host);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(port);
  addr_family = AF_INET;
  ip_protocol = IPPROTO_IP;
  inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);


  // implement a connection delay


    // increase the connection delay for next connection
  if (connection_delay == 0) {
    connection_delay = 1000;
  } else {
    connection_delay = connection_delay * 2;
  }

  // do not delay more that 16 seconds
  if (connection_delay > MAX_CONNECTION_DELAY) {
    connection_delay = MAX_CONNECTION_DELAY;
  }


  int sec = connection_delay / 1000;
  printf("will connect in %d seconds\n", sec);
  vTaskDelay(connection_delay / portTICK_PERIOD_MS);

  
  int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);


  if (sock < 0) {
    
    failed_socket_count++;

    printf("failed to create socket. Count=%d\n", failed_socket_count);
    // reboot if our failed socket count gets too high 
    if (failed_socket_count > MAX_FAILED_SOCKETS_TO_REBOOT) {
      esp_restart();
    }

    
    return sock;
  }

  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
 

  int cerr = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (cerr != 0) {
    printf("Connect Error.\n");
    return -1;
  } else {
    printf("Connected.\n");
    connection_delay = 0;
    return sock;
  }
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
// close the socket 
static int socket_disconnect(int sock) {
  // do the actual shutdown 
  shutdown(sock, 0);
  close(sock);
  return -1;
}


static void assert_frame(uint8_t *data, size_t len) {
  uint8_t c[6];

  c[0] = 2;
  c[1] = 68;
  c[2] = 65;
  c[3] = 84;
  c[4] = 65;
  c[5] = 3;



  if (len != 6) {
    printf("=============== >ASSERT FRAME: FAILED (wrong length)\n");
    return;
  }

  for(int i=0;i<6;i++){
    if(c[i] != data[i]) {
      printf("=============== >ASSERT FRAME: FAILED byte %d should be %d but is %d\n", i, c[i], data[i]);
      return;
      
    }
  }

  printf("=============== >ASSERT FRAME: PASS\n");

  

}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void executor_task( void *pvParameters ) {
  uint8_t rx_buf[ RX_BUF_SIZE];
  int nbytes;
  int sock = -1;
  EventBits_t flag;
  int ip_ok;

  // Wait to get an IP
  const TickType_t xTicksToWait = 10000 / portTICK_PERIOD_MS; 
  xEventGroupWaitBits(app_event_group,WIFI_GOT_IP,0,0,xTicksToWait);

  // initialize some vars 
  failed_socket_count = 0;
  protobuf_reset();


  // start main loop
  while (running) {
    flag = xEventGroupGetBits(app_event_group);
    ip_ok = (flag & WIFI_GOT_IP);

    // if we have an IP and no socket create a socket 
    if (ip_ok && (sock < 0)) {
      sock = socket_connect();
    }

    // if we have a socket and ip try read from the server ... 
    if (ip_ok && (sock >= 0)) {
      nbytes = read (sock, rx_buf,  RX_BUF_SIZE - 1);
      if (nbytes < 0){
        // socket in error state
        //printf("Read Timeout\n");
        //do nothing 
      } else if (nbytes == 0) {
        printf("Disconnected\n");
        sock = -1;
      } else {

        // test print the data that we read 
        printf ("--READ--\n[");
        for(int i=0;i<nbytes;i++) {
          printf("%d ",(uint8_t)rx_buf[i]);
        }
        printf("] %d bytes\n--END READ--\n\n", nbytes);
        
        // write to protobuf 
        protobuf_write(rx_buf,nbytes);

        // check for a frame 
        int fsize = protobuf_frame_size();
        
        if (fsize <  RX_BUF_SIZE) {
          if (fsize > 0) {
            // deframe 
            protobuf_deframe(rx_buf, fsize);
            printf ("--FRAME--\n[");
            for(int i=0;i<fsize;i++) {
              printf("%d ",rx_buf[i]);
            }
            printf("] %d bytes\n--END FRAME--\n\n", fsize);
            assert_frame(rx_buf, fsize);
          } 
        } else {
          printf("Out of Memory Error. Cannot deframe.\n");
        }
        
      }
    }   

    
    // Do we have any client instructions in the queue
    if (ip_ok && (sock >=0)) {
      // send client instructions to server 
    }
    
    // do we have and log events
    if (ip_ok && (sock >=0)) {
      // log to server 
    } else {
      // log to file 
    }
  

    // flush any log text file
    if (ip_ok && (sock >=0)) {
      // if we have a log file locally flush it to the server ...
    }
   

    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
  
  vTaskDelete(NULL);

}
