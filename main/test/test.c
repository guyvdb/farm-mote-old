#include "test.h"




#include "../event/event.h"






void task_print_wifi_ip(void *pvParam){
    printf("printWiFiIP task started \n");
    while(1){
      xEventGroupWaitBits(wifi_event_group,CONNECTED_BIT,true,true,portMAX_DELAY);
      tcpip_adapter_ip_info_t ip_info;
	    ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
	    printf("IP :  %s\n", ip4addr_ntoa(&ip_info.ip));

      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
