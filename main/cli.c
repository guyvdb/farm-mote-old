#include <stdio.h>

#include "cli.h"

static void task_cli(void *data);



/* ----------------------------------------------------------------------------
 *                                        
 * -------------------------------------------------------------------------- */
void task_create_cli(UBaseType_t priority) {
	xTaskCreate(task_cli, "cli", configMINIMAL_STACK_SIZE, NULL, priority, (xTaskHandle *) NULL);
}



/* ----------------------------------------------------------------------------
 *                                        
 * -------------------------------------------------------------------------- */
static void task_cli(void * data) {
  for(;;) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("task: cli\n");

  }
}
