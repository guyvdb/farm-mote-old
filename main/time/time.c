
#include "time.h"



#include <sys/time.h>
#include <stdio.h>

#include "../command/command.h"





/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
static void cmd_time(char *argv[], int argc, printfunc print) {

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
    print("time %s\n", buffer);
  } else {
    // Set Time 

     struct tm result;
     time_t time;
     struct timeval val;
     int rc;

    // "%Y-%m-%dT%H:%M:%S.%f"
    // "%a %m/%d/%Y %r"
    if (strptime(argv[1],"%Y-%m-%dT%H:%M:%S" ,&result) == NULL) {
      print("\nstrptime failed\n");
    } else {
      time = mktime(&result);
      val.tv_sec = time;
      val.tv_usec = 0;
      

      rc = settimeofday(&val, NULL);
      if (rc == 0) {
        print("time set\n");
      } else {
        print("time set failed with errno = %d\n",rc);
      }

    }

  }
}

/* ------------------------------------------------------------------------
 * 
 * --------------------------------------------------------------------- */
void initialize_time(void) {
  add_cmd("time",cmd_time,SOCKETINTERFACE | CONSOLEINTERFACE);
}



