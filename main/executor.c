#include "executor.h"
#include "command.h"

#include <frame.h>
#include <framebuf.h>
#include <framecon.h>
#include <framelst.h>

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>


#include <esp_log.h>
#include <esp_err.h>



#include <kv.h>
#include <console.h>
#include <command.h>
#include "wifi.h"


#define RX_BUF_SIZE 256
#define TIMEOUTDURATION 1000 // milliseconds

static int running = 0;
static QueueHandle_t txqueue;




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
      console_log_error("Executor failed to generate the mote id.");
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
static int set_gateway_info() {
  esp_err_t err;
  uint16_t port;
  char address[128];

  err = get_gateway_address(address, sizeof(address));
  if(err != ESP_OK) {
    printf("failed to get gateway address.\n");
    return 0;
  }

  err = get_gateway_port(&port);
  if (err != ESP_OK) {
    printf("failed to get gateway port.\n");
    return 0;
  }


  if(framecon_set_gateway_address(address) != 1) {
    printf("failed to set gateway address.\n");
    return 0;
  }

  if(framecon_set_gateway_port(port) != 1) {
    printf("failed to set gateway port.\n");
    return 0;
  }

  return 1;
}

/* ------------------------------------------------------------------------
 *
 * --------------------------------------------------------------------- */
void finalize_executor(void) {

}

/* ------------------------------------------------------------------------
 *
 * --------------------------------------------------------------------- */
void transmit(frame_t* frame) {
  xQueueSend(txqueue, &frame, 10);
}

/* ------------------------------------------------------------------------
 * The exector main task
 * --------------------------------------------------------------------- */
void executor_task( void *pvParameters ) {
  frame_t *rxframe = 0x0;
  frame_t *txframe = 0x0;
  frame_t *frame = 0x0;
  int timereqflag = 0;
  //uint32_t id = 5000;


  // Create the transmission queue -- max 10 frames
  txqueue =xQueueCreate(10, sizeof(frame_t*));

  // Wait to get an IP
  //  const TickType_t xTicksToWait = 10000 / portTICK_PERIOD_MS;
  //xEventGroupWaitBits(app_event_group,WIFI_GOT_IP,0,0,xTicksToWait);
  wifi_wait_for_interface();

  // After the wifi is up get the mote id. If it does not exist
  // it will be generated with the use of esp_random() which needs
  // the wifi to be running

  uint32_t moteid = get_mote_id();
  console_log_info("Mote ID = %d\n", moteid);

  // setup the gateway info
  set_gateway_info();



  // Generate an ident and time request
  frame = cmd_ident(moteid);
  transmit(frame);

  frame = cmd_time_request();
  transmit(frame);


  // start main loop
  while (running) {

    if(framecon_ready()) {
      // read a frame
      rxframe = framecon_read();
      if (rxframe != 0x0) {
        // debug some output
        char *msg = frame_to_string(rxframe);
        const char *cmd = command_to_string(rxframe->cmd);
        printf("RX %s %s\n",cmd, msg);
        free(msg);

        // process the frame
        switch(rxframe->cmd) {
        case TIMESET:
          if(cmd_time_set(rxframe)) {
            // success
            timereqflag = 1;
          }
          break;
        case ACK:
          // remove the frame from the resend list.. it is complete
          break;
        default:
          printf("CMD %d UNKNOWN\n",rxframe->cmd);
          break;
        }
        // free the frame
        frame_free(rxframe);
      }

      // write a frame
      if( xQueueReceive(txqueue, &txframe, 100)) {
        if(txframe != 0x0) {
          framecon_write(txframe);
          char *msg = frame_to_string(txframe);
          const char *cmd = command_to_string(txframe->cmd);
          printf("TX %s %s\n",cmd, msg);
          free(msg);
          frame_free(txframe);
        }
      }
    } else {
      // try connect again
      if(framecon_reconnect()) {
        frame = cmd_ident(moteid);
        transmit(frame);
        if(!timereqflag) {
          frame = cmd_time_request();
          transmit(frame);
        }
      }

    }

    vTaskDelay(TIMEOUTDURATION / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);

}





