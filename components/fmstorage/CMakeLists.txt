

idf_component_register(SRCS
  "storage.c"
  "filesystem.c"
  INCLUDE_DIRS .
  REQUIRES nvs_flash fatfs
  )
