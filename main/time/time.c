
#include "time.h"



#include <sys/time.h>
#include <stdio.h>

#include "../console/command.h"





/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_time(char *argv[], int argc) {

  if (argc == 1) {
  
    // Get Time
    struct timeval tv;
    time_t t;
    struct tm *ts;
    char buffer[80];
 
    gettimeofday(&tv, NULL);
    t = tv.tv_sec;

    ts = localtime(&t);


    // "%Y-%m-%dT%H:%M:%S"
    // "%a %Y-%m-%d %H:%M:%S %Z"
      
    strftime(buffer, sizeof(buffer),"%Y-%m-%dT%H:%M:%S" , ts);
    printf("time %s\n", buffer);
  } else {
    // Set Time 

     struct tm result;
     time_t time;
     struct timeval val;
     int rc;

    // "%Y-%m-%dT%H:%M:%S.%f"
    // "%a %m/%d/%Y %r"
    if (strptime(argv[1],"%Y-%m-%dT%H:%M:%S" ,&result) == NULL) {
      printf("\nstrptime failed\n");
    } else {
      time = mktime(&result);
      val.tv_sec = time;
      val.tv_usec = 0;
      

      rc = settimeofday(&val, NULL);
      if (rc == 0) {
        printf("time set\n");
      } else {
        printf("time set failed with errno = %d\n",rc);
      }

    }

  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_time(void) {
  add_console_cmd("time",cmd_time);
}



