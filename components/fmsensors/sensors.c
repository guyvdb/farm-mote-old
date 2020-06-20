#include "sensors.h"
#include "dht11.h"

#include <stdlib.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//#include <stddef.h>
//#include <stdint.h>


#define SLEEPDURATION 1000 * 1 // milliseconds

static sensor_t *root_sensor
;
static sensor_t *last_sensor;


static void sensor_add(const char *name, sensor_configure config, sensor_tick tick) {
  sensor_t *sensor = (sensor_t*)malloc(sizeof(sensor_t));
  sensor->name = (char*)malloc(strlen(name));
  sensor->config = config;
  sensor->tick = tick;
  strcpy(sensor->name, name);

  last_sensor->next = sensor;
  sensor->prev = last_sensor;
  sensor->next = 0x0;
  last_sensor = sensor;
}



static void sensor_task( void *pvParameters ) {


  printf("root sensor = 0x%X\n", (unsigned int)root_sensor);
  printf("next = 0x%X\n", (unsigned int)root_sensor->next);

  // initialize all the sensors
   sensor_t *sensor = root_sensor->next; // take roots next as first sensor




   printf("func=0x%X, &func=0x%X\n", (unsigned int ) dht11_configure, (unsigned int)&dht11_configure);


   while(sensor != 0x0) {

     printf("sensor = 0x%X\n", (unsigned int)sensor);
     printf("sensor->config = 0x%X\n", (unsigned int)sensor->config);
     sensor->config();
     sensor = sensor->next;

     /*   sensor = sensor->next; */
   }

  // start a 1 second tick
  while(1) {
        // call tick on all the sensors
    sensor = root_sensor->next;
    while(sensor != 0x0){
      sensor->tick();
      sensor = sensor->next;
    }
    vTaskDelay(SLEEPDURATION / portTICK_PERIOD_MS);
  }
}



void initialize_sensors(void){
  // add all the sensors to the linked list

  printf("add root sensor\n");

  // set up the root of our linked list 
  root_sensor = (sensor_t*)malloc(sizeof(sensor_t));
  root_sensor->name = (char*)malloc(5);
  strcpy(root_sensor->name,"ROOT");
  root_sensor->prev = 0x0;
  root_sensor->next = 0x0;
  root_sensor->config = 0x0;
  root_sensor->tick = 0x0;

  last_sensor = root_sensor;

  printf("add other sensors\n");

  // add all the sensors
  sensor_add("dht11", dht11_configure, dht11_tick);


  printf("start task\n");

  // start the sensor task
  xTaskCreate(sensor_task, "sensor_task", 2048, NULL, 5, NULL);
 
}
