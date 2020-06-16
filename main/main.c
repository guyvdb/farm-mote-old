#include <storage.h>
#include <filesystem.h>
#include <console.h>
#include <kv.h>
#include "wifi.h"
#include "executor.h"


void app_main(void) {
  initialize_console_commands();
  initialize_storage();
  initialize_filesystem();
  initialize_console();
  initialize_kv();
  initialize_wifi();
  initialize_executor();
    
  // enter the event loop of console. Control will not return until console is
  // stopped
  console_event_loop();


  // these are actually not being called. 
  finalize_console();
  finalize_executor();
  finalize_console_commands();
}
