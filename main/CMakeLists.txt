idf_component_register(SRCS
  "executor.c"
  "command.c"
  "wifi.c"
  "main.c"
  INCLUDE_DIRS "."
  REQUIRES fmframe fmconsole fmkv
  )
