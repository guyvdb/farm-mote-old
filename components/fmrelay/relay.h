#pragma once


#include <driver/gpio.h>

/* 
  This is the switch API. Switches are digital output. They can be 
  controled in series or parallel. The can be activated/deactivated 
  or toggles. They can also be activated for a set period of time 
  after which they will be deactivated.

  For example set switch 1 high for 5 minutes then low again.
*/


#define RELAY_MAXLEN 10  // The total number of switches defined

#define RELAY_PROCESS_UNKNOWN    0
#define RELAY_PROCESS_PARALLEL   1
#define RELAY_PROCESS_SERIAL     2




// This is the parameter passed to the switch_task 
struct {
} timed_toggle_task_t ;


// When the system is launched, configure a set of GPIO
// for use as switches 
void initialize_relays(void);

// Reconfigure the relay pins. Should be called is the pin values are
// changed via a console or server command 
void relay_reconfigure_pins(void);

// return the current state of the pin 
int relay_current_state(uint8_t pin);

// Set the given pin high 
void relay_on(uint8_t pin);

// Set the given pin low
void relay_off(uint8_t pin);


// If we are in the middle of a relay process return 1 else 0
int relay_is_process_running(void);

// Run a parallel sequence of switching. The task will
// set all pins high, wait duration seconds and then set all pins
// low. 
int relay_parallel_timed_toggle_task(uint32_t duration, uint8_t *pins, int len);

// Create a task to run a serial sequence of switching. The task will
// set all pins low and then cycle through each pin, seting it high for
// duration of millseconds before setting it low again. Once all pins
// have been cycled, the task will delete itself.
int relay_serial_timed_toggle_task(uint32_t duration, uint8_t *pins, int len);








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
