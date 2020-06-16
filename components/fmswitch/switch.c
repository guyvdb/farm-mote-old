#include "switch.h"

#include <stdio.h>




int switch_configure_gpio() {
  printf("switch_configure_gpio() called\n");
}


// Create a task to run a parallel sequence of switching. The task will
// set all pins high, wait duration milliseconds and then set all pins
// low. It will then delete itself.
int switch_parallel_timed_task(uint32_t duration, uint32_t *pins, int len) {
}

// Create a task to run a serial sequence of switching. The task will
// set all pins low and then cycle through each pin, seting it high for
// duration of millseconds before setting it low again. Once all pins
// have been cycled, the task will delete itself.
int switch_serial_timed_task(uint32_t duration, uint32_t *pins, int len) {
}




