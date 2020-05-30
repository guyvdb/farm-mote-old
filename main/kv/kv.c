#include <stdio.h>
#include <esp_err.h>
#include <string.h>
#include <tcpip_adapter.h>

#include "kv.h"
#include "../storage/storage.h"
#include "../command/command.h"



#define MAX_STRING_VALUE_LEN 128

static void cmd_wifi(char *argv[], int argc, printfunc print) {
  char value[MAX_STRING_VALUE_LEN];
  esp_err_t err;

  if (argc > 1) {
    if(strcmp(argv[1], "ip")==0) {     
      tcpip_adapter_ip_info_t ip_info;
	    ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
	    print("wifi ip %s\n", ip4addr_ntoa(&ip_info.ip));    
    } else if (strcmp(argv[1], "password")==0) {
      if (argc ==3) {
        err = set_wifi_password(argv[2]);
        if (err == ESP_OK) {
          print("wifi password *set*\n");
        } else {
          print("error %d %s\n", err, esp_err_to_name(err));
        }
      } else {
        err = get_wifi_password(value,sizeof(value));
        if (err == ESP_OK) {
          print("wifi password %s\n", value);
        } else {
          print("error %d  %s\n", err, esp_err_to_name(err));
        }
      }
    } else if (strcmp(argv[1], "ssid")==0) {
      if (argc ==3) {
        err = set_wifi_ssid(argv[2]);
        if (err == ESP_OK) {
          print("wifi ssid *set*\n");
        } else {
          print("error %d %s\n", err, esp_err_to_name(err));
        }
      } else {
        err = get_wifi_ssid(value,sizeof(value));
        if (err == ESP_OK) {
          print("wifi ssid %s\n", value);
        } else {
          print("error %d  %s\n", err, esp_err_to_name(err));
        }
      }
    } else {
      print("wifi [ssid|password|ip] <value>.\n");  
    }
  } else {
    print("wifi [ssid|password|ip] <value>.\n");  
  }
}




/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_gateway(char *argv[], int argc, printfunc print) {
  char value[MAX_STRING_VALUE_LEN];
  uint16_t port;
  esp_err_t err;

  if (argc > 1) {
     if (strcmp(argv[1], "address")==0) {
      if (argc ==3) {
        err = set_gateway_address(argv[2]);
        if (err == ESP_OK) {
          print("gateway address *set*\n");
        } else {
          print("error %d %s\n", err, esp_err_to_name(err));
        }
      } else {
        err = get_gateway_address(value,sizeof(value));
        if (err == ESP_OK) {
          print("gateway address %s\n", value);
        } else {
          print("error %d  %s\n", err, esp_err_to_name(err));
        }
      }
    } else if (strcmp(argv[1], "port")==0) {
      if (argc ==3) {
        port = atoi(argv[1]);
        err = set_gateway_port(port);
        if (err == ESP_OK) {
          print("gateway port *set*\n");
        } else {
          print("error %d %s\n", err, esp_err_to_name(err));
        }
      } else {
        err = get_gateway_port(&port);
        if (err == ESP_OK) {
          print("gateway port %d\n", port);
        } else {
          print("error %d  %s\n", err, esp_err_to_name(err));
        }
      }
    } else {
      print("gateway [address|port] <value>.\n");  
    }
  } else {
    print("gateway [address|port] <value>.\n");   
  }
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_kv(void) {
  // create all the commands for reading and writing kv values 
  add_cmd("wifi",cmd_wifi,CONSOLEINTERFACE);
  add_cmd("gateway",cmd_gateway, CONSOLEINTERFACE);
}

/* ------------------------------------------------------------------------
 * Get the wifi ssid 
 * --------------------------------------------------------------------- */
esp_err_t get_wifi_ssid(char *buf, size_t len) {
  return  read_storage_string_key("wifi.ssid",buf, len);
}

/* ------------------------------------------------------------------------
 * Set the wifi ssid 
 * --------------------------------------------------------------------- */
esp_err_t set_wifi_ssid(char *value) {
  return write_storage_string_key("wifi.ssid",value);
}


/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t get_wifi_password(char *buf, size_t len) {
  return read_storage_string_key("wifi.password",buf,len);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t set_wifi_password(char *value){
  return write_storage_string_key("wifi.password",value);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t get_gateway_address(char *buf, size_t len) {
  return read_storage_string_key("gateway.address",buf,len);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t set_gateway_address(char *value) {
  return write_storage_string_key("gateway.address",value);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t get_gateway_port(uint16_t *port) {
  return read_storage_uint16_key("gateway.port", port);
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
esp_err_t set_gateway_port(uint16_t port) {
  return write_storage_uint16_key("gateway.port", port);
}
