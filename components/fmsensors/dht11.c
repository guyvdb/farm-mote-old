#include "dht11.h"
#include "sensors.h"

#include <stdint.h>
#include <stdio.h>


#include <kv.h>
#include <command.h>
#include <log.h>
#include <frame.h>



static uint32_t counter = 0;

void dht11_configure(void) {
  printf("configure dht11\n");
}

void dht11_tick(void){
  counter ++;
  if(counter == 10) {
    counter = 0;


    frame_t *frame = log_create_sensor_reading(1,SENSOR_TYPE_DH11T, get_unix_timestamp() , 2);


    // append temp and humidity
    frame_args_put_uint8(frame,25);
    frame_args_put_uint8(frame,22);
    frame_args_end(frame);

    transmit(frame);

      // frame_t *log_create_sensor_reading(uint8_t pin, uint8_t sensortype, int32_t time, uint8_t paylen);

      // SENSOR_TYPE_DH11T

    printf("dht11 take reading\n");
  }
}
