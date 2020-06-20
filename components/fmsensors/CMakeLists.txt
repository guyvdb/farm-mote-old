

idf_component_register(SRCS
  "sensors.c"
  "dht11.c"
  INCLUDE_DIRS .
  REQUIRES fmkv fmcommand fmframe fmlog 
  )
