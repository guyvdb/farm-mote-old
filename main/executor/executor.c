#include "executor.h"
#include "frame.h"
#include "framebuf.h"
#include "network.h"
#include "command.h"


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

#include <sys/time.h>

#include "../kv/kv.h"
#include "../event/event.h"
#include "../console/log.h"
#include "../wifi/wifi.h"


#define RX_BUF_SIZE 256
static int running = 0;

static QueueHandle_t txqueue;
static int clocksetflag = 0;

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static uint32_t generate_mote_id(void) {
  esp_err_t err;
  uint8_t address;
  uint8_t result[4];
  uint32_t rand = esp_random();

  result[0] = rand >> 24;
  result[1] = rand >> 16;
  result[2] = rand >> 8;

  err = esp_base_mac_addr_get(&address);
  if (err != ESP_OK) {
    result[3] = rand;
  } else {
    result[3] = address;
  }
  return (result[0] << 24) + (result[1] << 16) + (result[2] << 8) + result[3];
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static uint32_t get_mote_id(void) {
  esp_err_t err;
  uint32_t value;

  err = get_id(&value);
  if (err != ESP_OK) {
    value = generate_mote_id();
    err = set_id(value);
    if (err != ESP_OK) {
      log_error("Executor failed to generate the mote id.");
      return 0;
    }
  }
  return value ;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_executor() {
  running = 1;
  xTaskCreate(executor_task, "executor_task", 4096, NULL, 5, NULL);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void finalize_executor(void) {

}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static uint32_t get_unix_time() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return tv.tv_sec;
}



/* ------------------------------------------------------------------------
 * The exector main task 
 * --------------------------------------------------------------------- */
void executor_task( void *pvParameters ) {
  frame_t *rxframe = 0x0;
  frame_t *txframe = 0x0;

  uint32_t id = 5000;
 

  // Create the transmission queue -- max 10 frames 
  txqueue =xQueueCreate(10, sizeof(frame_t*));

    
  // Wait to get an IP
  const TickType_t xTicksToWait = 10000 / portTICK_PERIOD_MS; 
  xEventGroupWaitBits(app_event_group,WIFI_GOT_IP,0,0,xTicksToWait);

  // After the wifi is up get the mote id. If it does not exist
  // it will be generated with the use of esp_random() which needs
  // the wifi to be running

  uint32_t moteid = get_mote_id();
  log_info("Mote ID = %d\n", moteid);
  
  // start main loop
  while (running) {

    if(wifi_valid()) {
      
      if (!socket_valid()) {
        printf("socket is not valid\n");
        framebuf_reset(); 
        socket_disconnect();
        socket_create();


        
        if(socket_connect()) {        
          // new connection to server. send ident 
          frame_t *ident = cmd_ident(moteid);
          xQueueSend(txqueue, &ident, 10);
        }
      }

      // do we need to set the system clock
      if(clocksetflag == 0) {
        frame_t *time = cmd_time_request();
        xQueueSend(txqueue, &time, 10);  
      }
      
      // Read a frame 
      if (socket_valid()) {
        rxframe = socket_read_frame();
        if (rxframe != 0x0) {

          printf("RX FRAME %d\n", rxframe->cmd);

          switch(rxframe->cmd) {
          case TIMESET:
            printf("CMD TIMESET\n");
            if(cmd_time_set(rxframe)) {
              clocksetflag = 1;
              printf("clocksetflag=%d\n", clocksetflag);
            }            
            break;
          default:
            printf("CMD %d UNKNOWN\n",rxframe->cmd);
            break;
          }
          frame_free(rxframe);
        }     
      }
      
      // Write a frame
      if( xQueueReceive(txqueue, &txframe, 100)) {        
        if(txframe != 0x0) {
          if(socket_valid()) {
            // send the frame to the server
            if(!socket_write_frame(txframe)) {
              log_error("Executor failed to write a frame.");
            } else {
              printf("TX FRAME %d\n", txframe->cmd);
            }
          } else {
            // socket is not connected ... need to log frame to disk 
          }          
          // delete the frame
          frame_free(txframe);
        }
      }        
    }


    vTaskDelay(6000 / portTICK_PERIOD_MS);
  }
  
  vTaskDelete(NULL);

}
