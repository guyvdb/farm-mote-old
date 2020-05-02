/*

  Farm Mote Project 

*/


#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "console/console.h"
#include "flash/flash.h"


#define CHIP_NAME "ESP32"


/* ----------------------------------------------------------------------------
 *                                        
 * -------------------------------------------------------------------------- */
void system_initialize() {
  system_initialize_flash();
  system_initialize_console();
}


/* ----------------------------------------------------------------------------
 *                                        
 * -------------------------------------------------------------------------- */
void system_create_tasks() {
  system_create_task_console(tskIDLE_PRIORITY + 1);
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
    //printf("task: main\n");
  }
  
}
