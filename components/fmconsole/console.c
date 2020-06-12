
#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <esp_console.h>
#include <linenoise/linenoise.h>
#include <argtable3/argtable3.h>
#include <driver/uart.h>
#include <esp_vfs_dev.h>
#include <esp_err.h>


#include "console.h"
#include "command.h"
#include "log.h"
#include <storage.h>

#define MOUNT_PATH "/data"

#define MAX_CMDLINE_LEN 256
#define MAX_CMDLINE_ARGS 10
static char *argv[MAX_CMDLINE_ARGS]; // the command line arguments


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static int parse_cmdline(char *cmdline) {
	char *delim = " ";
  int argc = 0;
	char *ptr = strtok(cmdline, delim);
  
	while(ptr != NULL) {
    argv[argc]=ptr;
		ptr = strtok(NULL, delim);
    argc++;
    if (argc >= MAX_CMDLINE_ARGS) {
      return argc-1;
    }
	}
  return argc;
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void finalize_console(void) {
  //free(cmdline_buf);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_console(void) {
  fflush(stdout);
  fsync(fileno(stdout));
  
  // Disable buffering on stdin 
  setvbuf(stdin, NULL, _IONBF, 0);

  // set CR as line ending 
  esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
  // end CRLF as tx ending  
  esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

  const uart_config_t uart_config = {
                                     .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
                                     .data_bits = UART_DATA_8_BITS,
                                     .parity = UART_PARITY_DISABLE,
                                     .stop_bits = UART_STOP_BITS_1,
                                     .source_clk = UART_SCLK_REF_TICK,
  };

  // Install UART driver for interrupt-driven reads and writes 
  ESP_ERROR_CHECK( uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0) );
  ESP_ERROR_CHECK( uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config) );

  // Tell VFS to use UART driver 
  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

  // Initialize the console 
  esp_console_config_t console_config = {
                                         .max_cmdline_args = 8,
                                         .max_cmdline_length = 256,
  };
  ESP_ERROR_CHECK( esp_console_init(&console_config) );


  // set up line noise 
  linenoiseSetMultiLine(0);
  linenoiseSetCompletionCallback(&esp_console_get_completion);
  linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);
  linenoiseHistorySetMaxLen(100);

  // alloc memory for the temp cmd line buf. This is freed in finalize_console();
  //cmdline_buf = calloc(MAX_CMDLINE_LEN,sizeof(char));
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void console_event_loop(void) {
  const char* prompt = "farm-mote> ";
  linenoiseSetDumbMode(1);
  while(true) {
    char* line = linenoise(prompt);
     
    if (line == NULL) { 
      continue;
    }

    // parse the command line 
    int argc = parse_cmdline(line);

    // find a command that matches argv[0]
    cmd_p cmd = find_console_cmd(argv[0]);

    if (cmd != NULL) {
      // find the print function that matches our interface   
      cmd->func(argv, argc);
    } else {
      printf("[ERROR] Command %s not found.\n", argv[0]);
    }

    linenoiseFree(line);
  }
}
