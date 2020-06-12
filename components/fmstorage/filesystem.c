#include "filesystem.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"



#define MOUNT_PATH "/data"

void initialize_filesystem(void) {
  static wl_handle_t wl_handle;
  const esp_vfs_fat_mount_config_t mount_config = {.max_files = 4, .format_if_mount_failed = true };
  esp_err_t err = esp_vfs_fat_spiflash_mount(MOUNT_PATH, "storage", &mount_config, &wl_handle);
  if (err != ESP_OK) {
    ESP_LOGE("FILESYSTEM", "Failed to mount FATFS (%s)", esp_err_to_name(err));
    return;
  }
}
