
#include "filesystem/filesystem.h"
#include "storage/storage.h"
#include "console/console.h"
#include "console/command.h"
#include "wifi/wifi.h"
#include "kv/kv.h"
#include "id/id.h"
#include "time/time.h"
#include "executor/executor.h"
#include "event/event.h"


void app_main(void) {

  
  initialize_console_commands();
  initialize_storage();
  initialize_filesystem();
  initialize_events();
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
  finalize_console_commands();
  finalize_events();
}
