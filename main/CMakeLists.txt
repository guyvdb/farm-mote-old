idf_component_register(SRCS
  "executor.c"
  "wifi.c"
  "main.c"
  INCLUDE_DIRS "."
  REQUIRES fmframe fmconsole fmkv fmcommand fmlog fmrelay fmsensors
  )
