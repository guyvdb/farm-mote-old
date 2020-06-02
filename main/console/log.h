#pragma once



// Instead of using printf, functions should call the log functions
// provided in this file. This allows logging to turned on/off
// interactivly on the console




// Turn console logging on/off (1/0)
void logging_on_off(int value);

// Turn the console prompt on/off (1/0)
void prompt_on_off(int value);

void log_error(const char *format, ...);
void log_std_error(const int err, const char *format, ...);
void log_info(const char *format, ...);
