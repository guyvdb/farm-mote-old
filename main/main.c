/*

  Farm Mote Project 

*/


#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cli.h"



#define CHIP_NAME "ESP32"


/* ----------------------------------------------------------------------------
 *                                        
 * -------------------------------------------------------------------------- */
void system_initialize() {
}


/* ----------------------------------------------------------------------------
 *                                        
 * -------------------------------------------------------------------------- */
void system_create_tasks() {
  task_create_cli(tskIDLE_PRIORITY + 1); // create the cli interface 
}


/* ----------------------------------------------------------------------------
 * Application entry point                                         
 * -------------------------------------------------------------------------- */
void app_main(void) {

  // initialize all components of the system
  system_initialize();

  
  // the main loop is responsible for creating all tasks
  system_create_tasks();


  
  // the main loop of the application 
  for(;;) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("task: main\n");
  }
  
}
