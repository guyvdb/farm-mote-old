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


void create_app_event_group() {
}

void app_main(void) {

  initialize_commands();
  initialize_storage();
  initialize_filesystem();
  initialize_console();
  initialize_kv();
  initialize_id();
  initialize_time();
  initialize_wifi();


  
  
  initialize_executor();
    
  // enter the event loop of console. Control will not return until console is
  // stopped
  console_event_loop();


  // these are actually not being called. 
  finalize_console();
  finalize_executor();
  finalize_commands();
  
}
