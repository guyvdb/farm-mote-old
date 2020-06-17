#include "console.h"

#include <kv.h>
#include <relay.h>
//#include <frame.h>
//#include <framecon.h>
//#include <log.h>


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_netif.h>
#include <esp_err.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define MAX_STRING_VALUE_LEN 128


cmd_p root_command;
cmd_p last_command;

static uint32_t get_timestamp(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  return (uint32_t)tv.tv_sec;
}


// ----------------------- START CONSOLE COMMANDS -------------------------


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_log(char *argv[], int argc){
  if (argc != 2) {
    console_log_error("Usage: log <on|off>.");
  } else {
    if (strcmp(argv[1],"on")==0) {
      console_logging_on_off(1);
      console_log_info("Logging on.");
    } else if(strcmp(argv[1], "off")==0) {
      console_logging_on_off(1);
      console_log_info("Logging off.");      
    } else {
      console_log_error("Usage: log <on|off>.");
    }
  }
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_prompt(char *argv[], int argc){
  if (argc != 2) {
    console_log_error("Usage: prompt <on|off>.");
  } else {
    if (strcmp(argv[1],"on")==0) {
      console_prompt_on_off(1);
      console_log_info("Prompt on.");
    } else if(strcmp(argv[1], "off")==0) {
      console_prompt_on_off(1);
      console_log_info("Prompt off.");      
    } else {
      console_log_error("Usage: prompt <on|off>.");
    }
  }
}



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_reboot(char *argv[], int argc){
  console_log_info("System will reboot in 1 second.");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  esp_restart();
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_id(char *argv[], int argc) {
  esp_err_t err;
  uint32_t id;
  
  if (argc == 1) {
    err = get_id(&id);
    if (err == ESP_OK) {
      printf("id %d\n", id);
    } else {
      printf("error %d  %s\n", err, esp_err_to_name(err));
    }    
  } else if (argc == 2) {
    id = (uint32_t)atoi(argv[1]);
    err = set_id(id);
    if (err == ESP_OK) {
      printf("id *set*\n");
    } else {
      printf("error %d  %s\n", err, esp_err_to_name(err));
    }        
      
  } else {
    console_log_error("Usage: id <id>.");
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_mem(char *argv[], int argc) {
  uint32_t fmem = esp_get_free_heap_size();
  printf("mem free %d\n", fmem);    
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_time(char *argv[], int argc) {

  if (argc == 1) {
    // Get time
    struct timeval now;
    gettimeofday(&now,0x0);
    time_t seconds = now.tv_sec;
    
    struct tm *local = localtime(&seconds);

   
    char buffer[64];
    struct tm timeinfo;

    localtime_r(&seconds, &timeinfo);
    strftime(buffer, sizeof(buffer),"%c",&timeinfo);

    printf("%s\n", buffer);
   
    
  } else if(argc == 2) {
    // Set Time 
     struct tm result;
     time_t time;
     struct timeval val;
     int rc;

    // "%Y-%m-%dT%H:%M:%S.%f"
    // "%a %m/%d/%Y %r"
    if (strptime(argv[1],"%Y-%m-%dT%H:%M:%S" ,&result) == NULL) {
      printf("\nstrptime failed\n");
    } else {
      time = mktime(&result);
      val.tv_sec = time;
      val.tv_usec = 0;
      

      rc = settimeofday(&val, NULL);
      if (rc == 0) {
        printf("time set\n");
      } else {
        printf("time set failed with errno = %d\n",rc);
      }

    }

  } else {
    console_log_error("Usage: time <RFC3339 time>.");
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_wifi(char *argv[], int argc) {
  char value[MAX_STRING_VALUE_LEN];
  esp_err_t err;

  if (argc > 1) {
    if(strcmp(argv[1], "ip")==0) {     
      tcpip_adapter_ip_info_t ip_info;
	    ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
	    printf("wifi ip %s\n", ip4addr_ntoa(&ip_info.ip));    
    } else if (strcmp(argv[1], "password")==0) {
      if (argc ==3) {
        err = set_wifi_password(argv[2]);
        if (err == ESP_OK) {
          printf("wifi password *set*\n");
        } else {
          printf("error %d %s\n", err, esp_err_to_name(err));
        }
      } else {
        err = get_wifi_password(value,sizeof(value));
        if (err == ESP_OK) {
          printf("wifi password %s\n", value);
        } else {
          printf("error %d  %s\n", err, esp_err_to_name(err));
        }
      }
    } else if (strcmp(argv[1], "ssid")==0) {
      if (argc ==3) {
        err = set_wifi_ssid(argv[2]);
        if (err == ESP_OK) {
          printf("wifi ssid *set*\n");
        } else {
          printf("error %d %s\n", err, esp_err_to_name(err));
        }
      } else {
        err = get_wifi_ssid(value,sizeof(value));
        if (err == ESP_OK) {
          printf("wifi ssid %s\n", value);
        } else {
          printf("error %d  %s\n", err, esp_err_to_name(err));
        }
      }
    } else {
      printf("wifi [ssid|password|ip] <value>.\n");  
    }
  } else {
    printf("wifi [ssid|password|ip] <value>.\n");  
  }
}




/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_gateway(char *argv[], int argc) {
  char value[MAX_STRING_VALUE_LEN];
  uint16_t port;
  esp_err_t err;

  if (argc > 1) {
     if (strcmp(argv[1], "address")==0) {
      if (argc ==3) {
        err = set_gateway_address(argv[2]);
        if (err == ESP_OK) {
          printf("gateway address *set*\n");
        } else {
          printf("error %d %s\n", err, esp_err_to_name(err));
        }
      } else {
        err = get_gateway_address(value,sizeof(value));
        if (err == ESP_OK) {
          printf("gateway address %s\n", value);
        } else {
          printf("error %d  %s\n", err, esp_err_to_name(err));
        }
      }
    } else if (strcmp(argv[1], "port")==0) {
      if (argc ==3) {
        port = atoi(argv[1]);
        err = set_gateway_port(port);
        if (err == ESP_OK) {
          printf("gateway port *set*\n");
        } else {
          printf("error %d %s\n", err, esp_err_to_name(err));
        }
      } else {
        err = get_gateway_port(&port);
        if (err == ESP_OK) {
          printf("gateway port %d\n", port);
        } else {
          printf("error %d  %s\n", err, esp_err_to_name(err));
        }
      }
    } else {
      printf("gateway [address|port] <value>.\n");  
    }
  } else {
    printf("gateway [address|port] <value>.\n");   
  }
}


/* ------------------------------------------------------------------------
 * The relay command has sub commands: pins, on, off
 * --------------------------------------------------------------------- */
static void cmd_relay(char *argv[], int argc) {
  esp_err_t err;
  uint8_t relays[RELAY_MAXLEN];  
  if(argc >= 2) {
    if(strcmp(argv[1], "pins") == 0) {
      if(argc == 2) {
        // get
        size_t len = get_relay_pin_count();
        err = get_relay_pins(&relays[0], RELAY_MAXLEN);
        if (err != ESP_OK) {
           printf("error %d %s\n", err, esp_err_to_name(err));
        } else {
          printf("relay pins ");
          for(int i=0;i<len;i++) {
            printf("%d ",relays[i]);
          }
          printf("\n");
        }
      } else {
        // set
        size_t len = argc - 2;
        for(int i = 0; i< len; i++) {
          relays[i] = (uint8_t)atoi(argv[i+2]); 
        }
        err = set_relay_pins(relays, len);
        if (err != ESP_OK) {
           printf("error %d %s\n", err, esp_err_to_name(err));
        } else {
          printf("relay pins *set*\n");
        }        
      }
    } else if (strcmp(argv[1], "on") == 0) {
      if(argc == 3) {
        uint8_t pin = atoi(argv[2]);
        relay_on(pin);
        printf("relay %d has been turned on.\n", pin);
      } else {
        printf("relay on <pin number>\n");
      }
    } else if (strcmp(argv[1], "off") == 0) {
      if(argc == 3) {
        uint8_t pin = atoi(argv[2]);
        relay_off(pin);
        printf("relay %d has been turned off.\n", pin);
      } else {
        printf("relay off <pin number>\n");
      }
    } else if (strcmp(argv[1],"state") == 0) {
      // get the relay state
      if(argc == 3) {
        uint8_t pin = atoi(argv[2]);
        uint8_t state = relay_current_state(pin);      
        if(state == 0) {
          printf("relay %d is off.\n", pin);
        } else if (state == 1) {
          printf("relay %d is on.\n", pin);
        } else {
          printf("Error: relay %d state is unknown.\n", pin);
        }
      } else {
        printf("relay state <pin number>\n");
      }


      // int relay_parallel_timed_toggle_task(uint32_t duration, uint8_t *pins, int len);
    } else if (strcmp(argv[1],"ptask") == 0) {
      // a parallel process task:   relay ptask duration-seconds pin pin ... pin
      if(argc < 4) {
        printf("relay ptask <duration-seconds> <pin> ... <pin>\n");
      } else {

        printf("args: [");
        for(int i=0;i<argc;i++) {
          if(i == argc-1) {
            printf("%d:%s",i,argv[i]);
          } else {
            printf("%d:%s | ",i,argv[i]);
          }
        }
        printf("]\n");
        
        uint32_t duration = atoi(argv[2]); // get duration 
        size_t len = argc - 3;             // figure out pin count 
        for(int i=0;i<len;i++) {           // get pins
          uint8_t pin = atoi(argv[i+3]);
          printf("pin %d: %d\n",i, pin);
          relays[i] = pin;
        }

        // fire the task
        relay_parallel_timed_toggle_task(duration, relays, len);
        
      }
    
    } else if (strcmp(argv[1],"stask") == 0) {
      // a serial process task  
      
    } else {
      printf("relay [pins|on|off|state|ptask|stask] <value(s)>.\n");  
    }
  } else {
    printf("relay [pins|on|off|state|ptask|stask] <value(s)>.\n");  
  }
}



// ------------------------ END CONSOLE COMMANDS --------------------------



/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void freecmd(cmd_p cmd) {
  free(cmd->name);
  free(cmd);
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_console_commands(void) {

  // set up the root dummy command 
  root_command = (cmd_p)malloc(sizeof(cmd_t));
  root_command->name = (char*)malloc(5);
  strcpy(root_command->name, "ROOT");
  root_command->prev = 0x0;
  root_command->next = 0x0;
  root_command->func = 0x0;
  last_command = root_command;


  // register all of the commands
  add_console_cmd("reboot",cmd_reboot);  
  add_console_cmd("id",cmd_id);
  add_console_cmd("time",cmd_time);
  add_console_cmd("wifi",cmd_wifi);
  add_console_cmd("gateway",cmd_gateway);
  add_console_cmd("log", cmd_log);
  add_console_cmd("prompt", cmd_prompt);
  add_console_cmd("mem", cmd_mem);
  add_console_cmd("relay", cmd_relay);
  
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void finalize_console_commands(void) {
  // delete all the commands 

  cmd_p prev;
  cmd_p current = last_command;


  while(current != 0x0) {
    if(current->prev != 0x0) {
      prev = (cmd_p)current->prev;
    } else {
      prev = 0x0;
    }

    freecmd(current);
    if (prev != 0x0) {
      current = prev;
    } else {
      current = 0x0;
    }
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
cmd_p add_console_cmd(char *name, cmdfunc *func) {
  // set up the new command
  cmd_p cmd = (cmd_p)malloc(sizeof(cmd_t));
  cmd->next = 0x0;
  cmd->prev = 0x0;
  cmd->name = (char *)malloc(strlen(name));
  strcpy(cmd->name, name);
  cmd->func = func;

  // the current last_command->next must now == new command
  last_command->next = (void*)cmd ;
  // the new command->prev must now == last command
  cmd->prev = (void*)last_command;    
  // last command must == new command 
  last_command = cmd;
  
  return cmd;
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
cmd_p find_console_cmd(char *name) {
  cmd_p ptr = root_command;

  while(1) {  
    if( strcmp(name, ptr->name) == 0) {
      return ptr;
    }

    if(ptr->next != 0x0) {
      ptr = (cmd_p) ptr->next;
    } else {
      return 0x0;
    } 
  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
//printfunc find_cmd_print_func(cmd_interface interface) {
//  return printf;
//}
