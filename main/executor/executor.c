#include "executor.h"
#include "frame.h"
#include "network.h"

#include <string.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>



#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_err.h>

#include "../kv/kv.h"
#include "../event/event.h"
#include "../console/command.h"
#include "../console/log.h"


#define RX_BUF_SIZE 256
static int running = 0;


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
// TODO: this must move out of this file into console/cmd_system.c
static void cmd_reboot(char *argv[], int argc){
  log_info("System will reboot in 1 second.");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  esp_restart();
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_executor() {
  running = 1;
  add_console_cmd("reboot",cmd_reboot);  
  xTaskCreate(executor_task, "executor_task", 4096, NULL, 5, NULL);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void finalize_executor(void) {

}

/* ------------------------------------------------------------------------
 * The exector main task 
 * --------------------------------------------------------------------- */
void executor_task( void *pvParameters ) {
  uint8_t rx_buf[ RX_BUF_SIZE];
  int nbytes;
  int sock = -1;
  EventBits_t flag;
  int ip_ok;
  
  // initialize some vars 
  framebuf_reset();


  // Wait to get an IP
  const TickType_t xTicksToWait = 10000 / portTICK_PERIOD_MS; 
  xEventGroupWaitBits(app_event_group,WIFI_GOT_IP,0,0,xTicksToWait);

  

  // start main loop
  while (running) {
    // do we have an IP 
    flag = xEventGroupGetBits(app_event_group);
    ip_ok = (flag & WIFI_GOT_IP);

    // if we have an IP and no socket create a socket 
    if (ip_ok && (sock < 0)) {
      sock =  socket_create();
      if (sock >= 0) {
        if(!socket_connect(sock)) {
          socket_destroy();
          sock = -1;
        }
      }
    }

    // if we have a socket and ip try read from the server ... 
    if (ip_ok && (sock >= 0)) {


      // ---------- READ ----------
      nbytes = read (sock, rx_buf,  RX_BUF_SIZE - 1);
      if (nbytes < 0){

        log_std_error(errno, "Networking read error.");


        if (errno != EAGAIN) {
          socket_disconnect(sock);
          socket_destroy();
          sock = -1;
        }
        
        /*
        switch(errno) {
        case EAGAIN:
          // Try again. This is a read timeout 
          printf("EAGAIN (try again)\n");
          break;
        case EBADF:
          // Bad file number. The socket should be destroyed and recreated
          printf("EBADF (bad file number)\n");
          socket_disconnect(sock);
          socket_destroy();
          sock = -1;
          break;
        case ECONNRESET:
          printf("ECONNRESET (connection reset by peer)\n");
          socket_disconnect(sock);
          socket_destroy();
          sock = -1;
          break;
        case EHOSTUNREACH :
          printf("EHOSTUNREACH (no route to host)\n");
          socket_disconnect(sock);
          socket_destroy();
          sock = -1;
          break;
        default:
          printf("UNKNOWN\n");

          socket_disconnect(sock);
          socket_destroy();
          sock = -1;
          break;
          
        }
        */
      } else if (nbytes == 0) {
        log_std_error(errno, "Networking is disconnected.");
        //        printf("Disconnected\n");
        socket_disconnect(sock);
        socket_destroy();
        sock = -1;
      } else {

        // test print the data that we read 
        printf ("--READ--\n[");
        for(int i=0;i<nbytes;i++) {
          printf("%d ",(uint8_t)rx_buf[i]);
        }
        printf("] %d bytes\n--END READ--\n\n", nbytes);
        
        // write to protobuf 
        framebuf_write(rx_buf,nbytes);

        // check for a frame 
        int fsize = framebuf_frame_size();
        
        if (fsize <  RX_BUF_SIZE) {
          if (fsize > 0) {


            //deframe
            framebuf_deframe(rx_buf, fsize);

            frame_t *frame = frame_from_bytes(rx_buf, fsize);
            frame_print(frame, rx_buf, fsize);
            frame_free(frame);
            
          } 
        } else {
          printf("Out of Memory Error. Cannot deframe.\n");
        }
        
      }
    }   



    // ---------- WRITE ----------
    
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
