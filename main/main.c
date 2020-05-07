#include "filesystem/filesystem.h"
#include "storage/storage.h"
#include "console/console.h"
#include "wifi/wifi.h"


void app_main(void) {
    initialize_storage();
    initialize_filesystem();
    initialize_console();


    initialize_wifi();

    

    
    
    // enter the event loop of console. This will not return
    console_event_loop();


    finalize_console();
}
