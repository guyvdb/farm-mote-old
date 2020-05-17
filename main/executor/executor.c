#include "executor.h"


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


#define HOST_IP_ADDR "192.168.8.100"
#define PORT 3000

static char host[128];
static uint16_t port;

static const char *TAG = "example";
static const char *payload = "PING\n";
static struct sockaddr_in dest_addr;
static int running = 0;

void initialize_executor(void) {
  running = 1;
  xTaskCreate(executor_task, "executor_task", 4096, NULL, 5, NULL);
}



void finalize_executor(void) {

}


// 

// Create and return the open socket 
static int open_socket_connection() {

  esp_err_t err;
  int addr_family;
  int ip_protocol;
  char addr_str[128];
  
  // gateway address
  err = get_gateway_address(host, sizeof(host));
  if (err != ESP_OK) {
     printf("[ERROR] %s\n", esp_err_to_name(err));
     return -1;
  }

  // gateway port 
  err = get_gateway_port(&port);
  if (err != ESP_OK) {
     printf("[ERROR] %s\n", esp_err_to_name(err));
     return -1;
  }

  
  dest_addr.sin_addr.s_addr = inet_addr(host);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(port);
  addr_family = AF_INET;
  ip_protocol = IPPROTO_IP;
  inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

    
  int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);

  
  
  if (sock < 0) {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    return sock;
  }
    // ESP_LOGI(TAG, "Socket created, connecting to %s:%d", HOST_IP_ADDR, PORT);

  int cerr = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (cerr != 0) {
    ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
    return -1;
  } else {
    ESP_LOGI(TAG, "Successfully connected");
    return sock;
  }
}

// close the socket 
static void close_socket_connection(int sock) {
}


void executor_task( void *pvParameters ) {

  

  char rx_buffer[128];
  

  printf("=======================================> executor_task started... will delay\n");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  printf("=======================================> delay over\n");


  int sock = open_socket_connection();
  
  while (running) {
    printf("+");
      
    int err = send(sock, payload, strlen(payload), 0);
    if (err < 0) {
      ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
     
    }

    /* int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
      if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
      } else {
        rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
        ESP_LOGI(TAG, "Received %d bytes", len);
        ESP_LOGI(TAG, "%s", rx_buffer);
      }
    */
      
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      printf("-\n");
    

      /*if (sock != -1) {
      printf("?????\n");
      ESP_LOGE(TAG, "Shutting down socket and restarting...");
      shutdown(sock, 0);
      close(sock);
      }*/
  }
  
  vTaskDelete(NULL);

}

