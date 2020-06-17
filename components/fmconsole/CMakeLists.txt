

idf_component_register(SRCS
  "console.c"
  "command.c"
  "log.c"
  INCLUDE_DIRS .
  REQUIRES fmkv fmrelay console
  )
