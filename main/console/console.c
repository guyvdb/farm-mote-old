
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "driver/uart.h"
#include "esp_vfs_dev.h"

#include "esp_err.h"


#include "console.h"
#include "../storage/storage.h"


#define MOUNT_PATH "/data"
//#define HISTORY_PATH MOUNT_PATH "/history.txt"

#define MAX_CMDLINE_LEN 256
#define MAX_CMDLINE_ARGS = 10
static char *argv[MAX_CMDLINE_ARGS]; // the command line arguments




static int parse_cmdline(char *cmdline) {
	int init_size = strlen(cmdline);
	char delim[] = " ";
  int argc = 0;
	char *ptr = strtok(cmdline, delim);

	while(ptr != NULL) {
    argv[argc]=ptr;
		printf("%s\n", ptr);
		ptr = strtok(NULL, delim);
    argc++;
    if (argc <= MAX_CMDLINE_ARGS) {
      return argc-1;
    }
	}
  return argc;
}



void finalize_console(void) {
  free(cmdline_buf);
}

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
  //linenoiseHistoryLoad(HISTORY_PATH);


  // alloc memory for the temp cmd line buf. This is freed in finalize_console();
  cmdline_buf = calloc(MAX_CMDLINE_LEN,sizeof(char));
}


cmd_t *add_cmd(char *name, cmdfunc *func) {
  cmd_t *cmd;

  cmd = (cmd_t*)calloc(
}


void console_event_loop(void) {
  const char* prompt = "farm-mote> ";
  linenoiseSetDumbMode(1);
  while(true) {
    char* line = linenoise(prompt);
     

    if (line == NULL) { 
      continue;
    }
       
      //linenoiseHistoryAdd(line);
      //linenoiseHistorySave(HISTORY_PATH);
  
      /* int ret; */
      /* esp_err_t err = esp_console_run(line, &ret); */
      /* if (err == ESP_ERR_NOT_FOUND) { */
      /*   printf("Unrecognized command\n"); */
      /* } else if (err == ESP_ERR_INVALID_ARG) { */
      /*   // command was empty */
      /* } else if (err == ESP_OK && ret != ESP_OK) { */
      /*   printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret)); */
      /* } else if (err != ESP_OK) { */
      /*   printf("Internal error: %s\n", esp_err_to_name(err)); */
      /* } */

    printf("\n[LINE] #%s#\n", line);

    strcpy(cmdline_buf, line);
    printf("[CMD LINE BUF] %s\n",cmdline_buf);


    int argc = parse_cmdline(cmdline_buf);

    for (int i=0;i<argc;i++){
      printf("%d) %s\n",i, argv[i]);
    }

    

    /*
    if(strcmp(line,"ssid") == 0 ) {
      printf("Get the ssid\n");
      char buf[64];
      esp_err_t err;

      err = get_wifi_ssid(buf,sizeof(buf));
      printf("err=%s, buf=%s\n",esp_err_to_name(err), buf);

    }
    */


    /* char **argv = (char **)calloc(MAX_CMDLINE_ARGS, sizeof(char*)); */
    /* if (argv == NULL) { */
    /*   printf("Failed to allocate argv buffer\n"); */
    /*   linenoiseFree(line); */
    /*   continue; */
    /* } */


    /* size_t argc = esp_console_split_argv(tmp_cmdline_buf, argv,  MAX_CMDLINE_ARGS); */

    /* printf("no args = %d\n", argc); */

    
      
      /* linenoise allocates line buffer on the heap, so need to free it */
    linenoiseFree(line);
  }
}




/*

esp_err_t esp_console_run(const char *cmdline, int *cmd_ret)
{
    if (s_tmp_line_buf == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    char **argv = (char **) calloc(s_config.max_cmdline_args, sizeof(char *));
    if (argv == NULL) {
        return ESP_ERR_NO_MEM;
    }
    strlcpy(s_tmp_line_buf, cmdline, s_config.max_cmdline_length);

    size_t argc = esp_console_split_argv(s_tmp_line_buf, argv,
                                         s_config.max_cmdline_args);
    if (argc == 0) {
        free(argv);
        return ESP_ERR_INVALID_ARG;
    }
    const cmd_item_t *cmd = find_command_by_name(argv[0]);
    if (cmd == NULL) {
        free(argv);
        return ESP_ERR_NOT_FOUND;
    }
    *cmd_ret = (*cmd->func)(argc, argv);
    free(argv);
    return ESP_OK;
}
*/
