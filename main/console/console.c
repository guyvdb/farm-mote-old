#include <stdio.h>
#include <stdlib.h>
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"

#include "console.h"

static void task_console(void *data);



/* ----------------------------------------------------------------------------
 *                                        
 * -------------------------------------------------------------------------- */
void system_initialize_console(void) {
  // drain stdout before reconfiguring
  fflush(stdout);
  fsync(fileno(stdout));

  // disable buffering on stdin
  setvbuf(stdin, NULL,_IONBF,0);

  // setup tx and rx line endings
  esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
  esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);


  //Configure UART. Note that REF_TICK is used so that the baud rate remains
  //correct while APB frequency is changing in light sleep mode.
     
  const uart_config_t uart_config = {
                                     .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE, //115200
                                     .data_bits = UART_DATA_8_BITS,
                                     .parity = UART_PARITY_DISABLE,
                                     .stop_bits = UART_STOP_BITS_1,
                                     .source_clk = UART_SCLK_REF_TICK,
  };


  // Install UART driver for interrupt-driven reads and writes 
  ESP_ERROR_CHECK( uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM,  256, 0, 0, NULL, 0) );
  ESP_ERROR_CHECK( uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config) );

  // Tell VFS to use UART driver 
  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

  // Initialize the console 
  esp_console_config_t console_config = {
                                         .max_cmdline_args = 8,
                                         .max_cmdline_length = 256,
  };
  ESP_ERROR_CHECK( esp_console_init(&console_config) );

  // Configure linenoise line completion library */
  // Enable multiline editing. If not set, long commands will scroll within
  // single line.
  linenoiseSetMultiLine(0);
  
  // Tell linenoise where to get command completions and hints 
  //linenoiseSetCompletionCallback(&esp_console_get_completion);
  //linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);
  
  // Set command history size 
  //linenoiseHistorySetMaxLen(100);
  
}




/* ----------------------------------------------------------------------------
 *                                        
 * -------------------------------------------------------------------------- */
void system_create_task_console(UBaseType_t priority) {
	xTaskCreate(task_console, "console", 1024*4, NULL, priority, (xTaskHandle *) NULL);
  printf("--------------------> SYSTEM STACK = %d\n", configMINIMAL_STACK_SIZE);
}



/* ----------------------------------------------------------------------------
 *                                        
 * -------------------------------------------------------------------------- */
static void task_console(void * data) {
  //char *line;
  //char *prompt = "farm-mote>";

  UBaseType_t uxHighWaterMark;
  char[32] buffer;
  
  for(;;) {



    /* while((line = linenoise(prompt)) != NULL) { */
    /*   printf("You wrote: %s\n", line); */
    /*   linenoiseFree(line);  */
    /* } */


    
    vTaskDelay(1000 / portTICK_PERIOD_MS);


    UBaseType_t hw = uxTaskGetStackHighWaterMark(NULL);
    

    
    //  uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    // printf("high water = %d\n", uxHighWaterMark);

    itoa(hw,buffer,31);

    printf("console...%s\n", buffer);
  }
}
