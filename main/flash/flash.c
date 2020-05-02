
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "flash.h"



static char namespace[16] = "storage";


void system_initialize_flash(void) {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK( nvs_flash_erase() );
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
}



