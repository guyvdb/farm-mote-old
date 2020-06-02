#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <lwip/err.h>

static int logstatus = 1;
static int promptstatus = 1;


// Turn console logging on/off (1/0)
void logging_on_off(int value) {
  logstatus = value;
}

// Turn the console prompt on/off (1/0)
void prompt_on_off(int value){
  promptstatus = value;
}



void log_std_error(const int err, const char *format, ...) {
  va_list args;
  const char *prompt = "farm-mote>";

  if (logstatus) {
    if(promptstatus) {
      printf("\n");
    }

    printf("[ERROR] ");
    va_start(args, format);
    printf(format, args);
    va_end(args);
    printf(" errno = %d. ", err);

    switch(errno) {
    case EAGAIN:
      printf("EAGAIN (try again)");
      break;
    case EBADF:
      printf("EBADF (bad file number)");
      break;
    case ECONNRESET:
      printf("ECONNRESET (connection reset by peer)");
      break;
    case EHOSTUNREACH :
      printf("EHOSTUNREACH (no route to host)");
      break;
    case ENFILE:
      printf("ENFILE (File table overflow)");
      break;
    default:
      printf("UNKNOWN");
      break;      
    }

    printf("\n");

    if (promptstatus) {
      printf("%s", prompt);
    }
  }
  
}


static void log(const char *level, const char *format, ...) {
  va_list args;
  const char* prompt = "farm-mote> ";

  // are we displaying the log 
  if(logstatus) {
    if(promptstatus) {
      printf("\n"); // get off the prompt line 
    }

    printf("[%s] ",level);
    va_start(args, format);
    printf(format, args);
    va_end(args);
    printf("\n");

    if (promptstatus) {
      printf("%s", prompt);
    }
  }

  
}

void log_error(const char *format, ...) {
  va_list args;

  va_start(args,format);
  log("ERROR",format,args);
  va_end(args);
}

void log_info(const char *format, ...) {
  va_list args;

  va_start(args,format);
  log("INFO",format,args);
  va_end(args);
}