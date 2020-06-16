#pragma once


#include <driver/gpio.h>

/* 
  This is the switch API. Switches are digital output. They can be 
  controled in series or parallel. The can be activated/deactivated 
  or toggles. They can also be activated for a set period of time 
  after which they will be deactivated.

  For example set switch 1 high for 5 minutes then low again.



*/


#define SWITCH_COUNT 10  // The total number of switches defined 

typedef enum {
              SWITCH_UNKOWN_PROCESS = 0,
              SWITCH_PARALLEL_PROCESS = 1,
              SWITCH_SERIAL_PROCESS = 2,
} switch_processing_sequence_t;


struct {
  gpio_num_t gpio;
  uint8_t state;
} switch_t;

//I (326) gpio: GPIO[19]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 


// The current state of all switches 
struct {
  switch_t switches[SWITCH_COUNT];
} switch_state_t;



// When the system is launched, configure a set of GPIO
// for use as switches 
int switch_configure_gpio();

// Create a task to run a parallel sequence of switching. The task will
// set all pins high, wait duration milliseconds and then set all pins
// low. It will then delete itself.
int switch_parallel_timed_task(uint32_t duration, uint32_t *pins, int len);

// Create a task to run a serial sequence of switching. The task will
// set all pins low and then cycle through each pin, seting it high for
// duration of millseconds before setting it low again. Once all pins
// have been cycled, the task will delete itself.
int switch_serial_timed_task(uint32_t duration, uint32_t *pins, int len);








/*
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
*/
