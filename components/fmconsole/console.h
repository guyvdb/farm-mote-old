
#pragma once

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>


void initialize_console(void);
void finalize_console(void);
void console_event_loop(void);

void initialize_console_commands(void);
void finalize_console_commands(void);


// Log related

// Turn console logging on/off (1/0)
void console_logging_on_off(int value);

// Turn the console prompt on/off (1/0)
void console_prompt_on_off(int value);

void console_log_error(const char *format, ...);
void console_log_std_error(const int err, const char *format, ...);
void console_log_info(const char *format, ...);
void console_log_info_uint8_array(uint8_t *data, size_t len, const char *format, ...);



