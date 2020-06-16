#include <storage.h>
#include <filesystem.h>
#include <console.h>
#include <kv.h>
#include <switch.h>
#include "wifi.h"
#include "executor.h"


void app_main(void) {
  initialize_console_commands();
  initialize_storage();
  initialize_filesystem();
  initialize_console();
  initialize_kv();
  initialize_switches();
  initialize_wifi();
  initialize_executor();  // A task will be created 
    

  console_event_loop();   // This runs within this task and will block 


  // TODO:  these are actually not being called. 
  finalize_console();
  finalize_executor();
  finalize_console_commands();
}
