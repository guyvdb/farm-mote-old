#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "filesystem/filesystem.h"
#include "storage/storage.h"
#include "console/console.h"
#include "wifi/wifi.h"
#include "command/command.h"
#include "kv/kv.h"
#include "id/id.h"
#include "time/time.h"
#include "executor/executor.h"



#define WIFI_INITIALIZED   BIT0
#define WIFI_CONNECTED     BIT1
#define WIFI_GOT_IP        BIT2





EventGroupHandle_t app_event_group;

void app_main(void) {

  app_event_group = xEventGroupCreate();
  

  initialize_commands();
  initialize_storage();
  initialize_filesystem();
  initialize_console();
  initialize_kv();
  initialize_id();
  initialize_time();
  initialize_wifi(app_event_group);


  
  
  initialize_executor(app_event_group);
    
  // enter the event loop of console. Control will not return until console is
  // stopped
  console_event_loop();


  vEventGroupDelete(app_event_group);

  // these are actually not being called. 
  finalize_console();
  finalize_executor();
  finalize_commands();
  
}
