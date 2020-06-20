#include <esp_log.h>

#include <storage.h>
#include <filesystem.h>
#include <console.h>
#include <kv.h>
#include <relay.h>
#include <sensors.h>

#include "wifi.h"
#include "executor.h"


void app_main(void) {

  esp_log_level_set("*", ESP_LOG_NONE);

  initialize_console_commands();
  initialize_storage();
  initialize_filesystem();
  initialize_console();
  initialize_kv();

  initialize_relays();


  initialize_wifi();
  initialize_executor();  // A task will be created

  
  //initialize_sensors();


  console_event_loop();   // This runs within this task and will block


  // TODO:  these are actually not being called.
  finalize_console();
  finalize_executor();
  finalize_console_commands();
}
