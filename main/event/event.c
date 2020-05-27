#include "event.h"


/* The application event group */
EventGroupHandle_t app_event_group;



void initialize_events(void) {
  app_event_group = xEventGroupCreate();
}

void finalize_events(void) {
  vEventGroupDelete(app_event_group);
}

/*
EventGroupHandle_t get_app_event_group(void) {
  return app_event_group;
}

EventBits_t set_app_event_bits( const EventBits_t uxBitsToSet ) {
  return xEventGroupSetBits(app_event_group,uxBitsToSet);
}



EventBits_t clear_app_event_bits(const EventBits_t uxBitsToClear ) {
  return xEventGroupClearBits(app_event_group, uxBitsToClear);
}
*/
