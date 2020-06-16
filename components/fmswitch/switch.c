#include "switch.h"

#include <stdio.h>


// Configure the pins 
static void configure_gpio(uint64_t mask) {
}

// This is the timed task that will execute the 
static void switch_task( void *pvParameters ) {
  
}

void initialize_switches(void) {
  printf("switch_configure_gpio() called\n");

  // #define GPIO_OUTPUT_PIN_SEL (1ULL<<GPIO_OUTPUT_IO_0) // ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
  uint64_t mask = 0;

  // get a list of the configured gpio ports from the kv store


  
  
  
}

// Create a task to run a parallel sequence of switching. The task will
// set all pins high, wait duration milliseconds and then set all pins
// low. It will then delete itself.
int switch_parallel_timed_task(uint32_t duration, uint32_t *pins, int len) {
  return 0;
}

// Create a task to run a serial sequence of switching. The task will
// set all pins low and then cycle through each pin, seting it high for
// duration of millseconds before setting it low again. Once all pins
// have been cycled, the task will delete itself.
int switch_serial_timed_task(uint32_t duration, uint32_t *pins, int len) {
  return 0;
}










