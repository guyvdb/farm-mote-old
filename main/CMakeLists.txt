idf_component_register(SRCS
  "executor/executor.c"
  "executor/command.c"
  "wifi/wifi.c"
  "main.c"
  INCLUDE_DIRS "."
  REQUIRES fmframe fmconsole fmkv
  )
