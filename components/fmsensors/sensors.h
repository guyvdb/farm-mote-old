#pragma once

#include <frame.h>

// This file sets up a sensors task and includes various sensor drivers that can
// be executed within the task.


// The tasks ticks at a resolution of 1 second and each driver can determine if it
// should log on the tick or not. This means that reading can be performed in single
// second resolution

// The drivers need to set up two functions with the task. They are sensor_configure
// and sensor_tick.


#define SENSOR_TYPE_DH11T   0


extern void transmit(frame_t* frame); // forward declare method in executor.c


typedef void (sensor_configure)(void);
typedef void (sensor_tick)(void);

struct sensor;

struct sensor {
  char *name;
  struct sensor *prev;
  struct sensor *next;
  sensor_configure *config;
  sensor_tick *tick;
};

typedef struct sensor sensor_t;

void initialize_sensors(void);


