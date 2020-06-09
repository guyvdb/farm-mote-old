idf_component_register(SRCS
  "filesystem/filesystem.c"
  "storage/storage.c"
  "console/console.c"
  "console/command.c"
  "console/log.c"
  "executor/executor.c"
  "executor/network.c"
  "executor/command.c"
  "relay/relay.c"
  "wifi/wifi.c"
  "state/state.c"
  "kv/kv.c"
  "event/event.c"
  "main.c"
  INCLUDE_DIRS "."
  REQUIRES frame
  )
