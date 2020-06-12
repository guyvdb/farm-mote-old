idf_component_register(SRCS
  "filesystem/filesystem.c"
  "executor/executor.c"
  "executor/command.c"
  "relay/relay.c"
  "wifi/wifi.c"
  "state/state.c"
  "event/event.c"
  "main.c"
  INCLUDE_DIRS "."
  REQUIRES fmframe fmconsole fmkv
  )
