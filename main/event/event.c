#include "event.h"


/* The application event group */
EventGroupHandle_t app_event_group;



void initialize_events(void) {
  app_event_group = xEventGroupCreate();
}

void finalize_events(void) {
  vEventGroupDelete(app_event_group);
}
