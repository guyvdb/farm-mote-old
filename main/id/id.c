#include <stdio.h>
#include "esp_system.h"
#include "id.h"

#include "../command/command.h"



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_id(char *argv[], int argc, printfunc print) {
  esp_err_t err;
  uint8_t address;
  err = esp_base_mac_addr_get(&address);
  print("id %d\n", address);
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_id(void) {
  // create all the commands for reading and writing kv values 
  add_cmd("id",cmd_id,SOCKETINTERFACE | CONSOLEINTERFACE);
}

 
