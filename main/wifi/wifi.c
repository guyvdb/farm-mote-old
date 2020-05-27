#include "wifi.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "../kv/kv.h"

#include "../command/command.h"
#include "../event/event.h"



#define EXAMPLE_ESP_MAXIMUM_RETRY  2





static char wifi_ssid[32];
static char wifi_password[64];
//static EventGroupHandle_t app_event_group;
static const char *TAG = "WIFI";

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_wifi(char *argv[], int argc, printfunc print){

  if (argc != 2) {
    printf("[ERROR] wrong number of arguments. wifi start | wifi stop.\n");
  } else {
    if (strcmp("start",argv[1]) == 0) {
      wifi_ap_connect();
    } else if (strcmp("stop",argv[1]) == 0) {
      wifi_ap_disconnect();
    } else {
      printf("[ERROR] unknown argument. wifi start | wifi stop.\n");
    }
  }
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    switch (event_id) {
    case SYSTEM_EVENT_STA_START:
 	    ESP_LOGI(TAG, "STA_START %s %d\n", __func__, __LINE__);

      //set_app_event_bits(WIFI_INITIALIZED);

      
      xEventGroupSetBits(app_event_group, WIFI_INITIALIZED);
 	    ESP_LOGI(TAG, "Connecting..\n");
 	    esp_wifi_connect();
      break;
    case SYSTEM_EVENT_STA_STOP:
 	    ESP_LOGI(TAG, "STA_STOP %s %d\n", __func__, __LINE__);

      //clear_app_event_bits(WIFI_INITIALIZED);

      xEventGroupClearBits(app_event_group, WIFI_INITIALIZED);
 	    ESP_ERROR_CHECK( esp_wifi_deinit());
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
 	    ESP_LOGI(TAG, "STA_GOT_IP %s %d\n", __func__, __LINE__);
      //set_app_event_bits(WIFI_GOT_IP);

      xEventGroupSetBits(app_event_group, WIFI_CONNECTED);
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
 	    ESP_LOGI(TAG, "STA_LOST_IP %s %d\n", __func__, __LINE__);

      //clear_app_event_bits(WIFI_CONNECTED);
      xEventGroupClearBits(app_event_group, WIFI_CONNECTED);
 	    break;
    case SYSTEM_EVENT_STA_CONNECTED:
 	    ESP_LOGI(TAG, "STA_CONNECTED %s %d\n", __func__, __LINE__);
 	    break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
 	    ESP_LOGI(TAG, "STA_DISCONNECTED %s %d\n", __func__, __LINE__);

      //clear_app_event_bits(WIFI_CONNECTED);
      xEventGroupClearBits(app_event_group, WIFI_CONNECTED);
      break;
    default:
 	    ESP_LOGI(TAG, "XXX_DEFAULT %s %d\n", __func__, __LINE__);
      break;
    }
}



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void wifi_ap_initialize() {
  //esp_err_t err ;


  // create event group 
  //app_event_group = xEventGroupCreate();


  // init wifi 
  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));


  // setup ssid + password
  get_wifi_ssid(wifi_ssid, sizeof(wifi_ssid));
  get_wifi_password(wifi_password, sizeof(wifi_password));

  wifi_config_t wifi_config = {.sta = {} };

  for(int i=0;i<sizeof(wifi_ssid);i++) {
    wifi_config.sta.ssid[i] = wifi_ssid[i];
  }

  for(int i=0;i<sizeof(wifi_password);i++) {
    wifi_config.sta.password[i] = wifi_password[i];
  }


  // start wifi 
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
  ESP_ERROR_CHECK(esp_wifi_start() );


  // unregister event handlers
  ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
  ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void wifi_ap_connect(void) {
  //ESP_ERROR_CHECK(esp_wifi_start() );
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void wifi_ap_disconnect(void) {
  //  ESP_ERROR_CHECK(esp_wifi_stop());
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_wifi(void) {
   add_cmd("wifi",cmd_wifi, CONSOLEINTERFACE);

   //app_event_group = events;
   
   wifi_ap_initialize();
   wifi_ap_connect();
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void deinitialize_wifi(void) {


}
