#ifndef CONSOLE_H
#define CONSOLE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void system_initialize_console(void);
void system_create_task_console(UBaseType_t priority);


#endif
