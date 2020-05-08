#include "filesystem/filesystem.h"
#include "storage/storage.h"
#include "console/console.h"
#include "wifi/wifi.h"
#include "command/command.h"
#include "kv/kv.h"
#include "id/id.h"

void app_main(void) {

  initialize_commands();
  initialize_storage();
  initialize_filesystem();
  initialize_console();
  initialize_kv();
  initialize_id();
  
  //initialize_wifi();
    
    
  // enter the event loop of console.
  console_event_loop();


  finalize_console();
  finalize_commands();
}
