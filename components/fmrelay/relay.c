#include "relay.h"

#include <stdio.h>

#include <kv.h>
#include <command.h>
#include <log.h>
#include <frame.h>
#include <framecon.h>

#include <esp_err.h>
#include <esp_timer.h>


static uint8_t active_pins[RELAY_MAXLEN];
static size_t active_pincount = 0;
static int relay_process = RELAY_PROCESS_UNKNOWN;
static int pin_ptr = 0;
static uint32_t process_duration;
static int process_running = 0;

static int32_t task_start_time = 0;
static int32_t pin_start_time = 0;

static esp_timer_handle_t timer;



static void parallel_timed_task_callback(void* arg) {
  frame_t *frame;

  // task end time 
  int32_t task_end_time = get_unix_timestamp();
  
 
  // set all pins to level 0
  for(int i=0;i<active_pincount;i++) {
    gpio_set_level(active_pins[i], 0);
  }
  
  // log all pin changes + task complete 
  for(int i=0; i< active_pincount;i++) {
    // pin state change log
    if(framecon_write(log_create_relay_state_change(active_pins[i], 0, task_end_time)) == 0 ) {
      printf("Error: failed to log relay %d state change to %d\n", active_pins[i], 0);
    }
    
    
    // pin task complete log
    if(framecon_write(log_create_relay_timed_toggle_complete(active_pins[i], task_start_time,task_end_time)) == 0) {
      printf("Error: failed to log relay %d timed toggle complete\n", active_pins[i]);
    }
  }

  // delete the timer
  esp_timer_delete(timer);

}

static void serial_timed_task_callback(void* arg) {
  // set current pin to level 0

  // log pin change

  // start next pin if needed

  // if all complete log task complete

  // if all complete stop and delete the timer 
  
}



void initialize_relays(void) {
  relay_reconfigure_pins();
}

void relay_reconfigure_pins(void) {
  esp_err_t err;
  uint8_t pins[RELAY_MAXLEN];
  gpio_config_t io_conf;
 
  size_t pincount = get_relay_pin_count();

  err =  get_relay_pins(pins, RELAY_MAXLEN);
  if (err != ESP_OK) {
    printf("ERROR cannot reconfigure relay pins.\n");
    return;
  }

  // for now we configure each pin individually.
  // need to do this as a single operation
  for(int i=0;i<pincount;i++) {
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;

    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;//GPIO_MODE_OUTPUT_OD;

    //bit mask of the pin
    io_conf.pin_bit_mask = (1ULL << pins[i]);

    //disable pull-down mode
    io_conf.pull_down_en = 0;

    //disable pull-up mode
    io_conf.pull_up_en = 0;

    //configure GPIO with the given settings
    gpio_config(&io_conf);
  }
}
/*
The question was asked several times already, you must set the gpio mode to GPIO_MODE_INPUT_OUTPUT (or GPIO_MODE_INPUT_OUTPUT_OD)
 if you want to use the gpio as output and read its state back. Or you can simply save the state in a variable after gpio_set_level().
*/


// return the current state of the pin 
int relay_current_state(uint8_t pin) {
  return gpio_get_level(pin);
}

// Set the given pin high 
void relay_on(uint8_t pin) {
  gpio_set_level(pin, 1);
  if(framecon_write(log_create_relay_state_change(pin, 1, get_unix_timestamp())) == 0 ) {
    printf("Error: failed to log relay %d state change to %d\n", pin, 1);
  }   
}

// Set the given pin low
void relay_off(uint8_t pin) {
  gpio_set_level(pin, 0);
  if(framecon_write(log_create_relay_state_change(pin, 0, get_unix_timestamp())) == 0 ) {
    printf("Error: failed to log relay %d state change to %d\n", pin, 0);
  }     
}


// If we are in the middle of a relay process return 1 else 0
int relay_is_process_running(void) {
  return process_running;
}


// Run a parallel sequence of switching. The task will
// set all pins high, wait duration seconds and then set all pins
// low. 
int relay_parallel_timed_toggle_task(uint32_t duration, uint8_t *pins, int len) {
  

  if(len > RELAY_MAXLEN) {
    printf("ERROR too many relays.\n");
    return 0;
  }

  printf("ptask - pins param: [");
  for(int i=0;i<len;i++) {
    if(i == len -1) {
      printf("%d",pins[i]);
    } else {
      printf("%d ",pins[i]);
    }
  }
  printf("]\n");

  

  // the duration is in seconds. the timer task needs microseconds.
  uint64_t timeout =    duration * 1000 * 1000; //   ((uint64_t)duration)*1000*1000); // seconds * 1000 = ms * 1000 = us

  // set the len of active pins
  active_pincount = len;

  // mark the process as running
  process_running = 1;
  
  // set the process start time
  task_start_time = get_unix_timestamp();

  // Setup timer params 
  const esp_timer_create_args_t timer_args = {.callback = &parallel_timed_task_callback,.name = "parallel_timer"};
  
  // Create the timer 
  ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));

  // set all pins high + record pin as active 
  for(int i=0; i< len; i++) {
    active_pins[i] = pins[i];    // mark as active
    gpio_set_level(pins[i], 1);  // set pin high
  }

  // start the timer
  ESP_ERROR_CHECK(esp_timer_start_once(timer, timeout));

  // log all state changes
  for(int i=0;i<len;i++) {
    if(framecon_write(log_create_relay_state_change(pins[i], 1, task_start_time)) == 0 ) {
      printf("Error: failed to log relay %d state change to %d\n", pins[i], 0);
    }    
  }


  printf("ptask - active pins: [");
  for(int i=0;i<active_pincount;i++) {
    if(i == active_pincount -1) {
      printf("%d",active_pins[i]);
    } else {
      printf("%d ",active_pins[i]);
    }
  }
  printf("]\n");
  
  
  // callback will end the process when timer expires  
  return 1;
}

// Create a task to run a serial sequence of switching. The task will
// set all pins low and then cycle through each pin, seting it high for
// duration of millseconds before setting it low again. Once all pins
// have been cycled, the task will delete itself.
int relay_serial_timed_task(uint32_t duration, uint8_t *pins, int len) {
  return 0;
}










