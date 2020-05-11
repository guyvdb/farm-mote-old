#include "time.h"



#include <sys/time.h>
#include <stdio.h>

#include "../command/command.h"




// Time needs to work on RFC3339 standard
// 2019-10-12T07:20:50.52Z



/*
Convert from human-readable date to epoch
#include <stdio.h>
#include <time.h>

int main(void) {
    struct tm t;
    time_t t_of_day;

    t.tm_year = 2019-1900;  // Year - 1900
    t.tm_mon = 7;           // Month, where 0 = jan
    t.tm_mday = 8;          // Day of the month
    t.tm_hour = 16;
    t.tm_min = 11;
    t.tm_sec = 42;
    t.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = mktime(&t);

    printf("seconds since the Epoch: %ld\n", (long) t_of_day);
}
*/


/*

Convert from epoch to human-readable date
How to convert epoch / UNIX timestamps to normal readable date/time using C.

Example C routine using STRFTIME. STRFTIME converts information from a time structure to a string form, and writes the string into the memory area pointed to by "string".

#include <stdio.h>
#include <time.h>

int main(void)
{
    time_t     now;
    struct tm  ts;
    char       buf[80];

    // Get current time
    time(&now);

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
    printf("%s\n", buf);
    return 0;
}

*/


static void cmd_time(char *argv[], int argc) {
  //  struct timeval current_time;
  //gettimeofday(&current_time, NULL);
  //printf("seconds : %ld\nmicro seconds : %ld", current_time.tv_sec, current_time.tv_usec);


  if (argc == 1) {
  
  // Get
    struct timeval tv;
    time_t t;
    struct tm *ts;
    char buffer[80];
 
    gettimeofday(&tv, NULL);
    t = tv.tv_sec;

    ts = localtime(&t);



    strftime(buffer, sizeof(buffer), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    printf("%s\n", buffer);


    /*
    printf("%s",asctime (info));
    strftime (buffer, sizeof buffer, "Today is %A, %B %d.\n", info);
    printf("%s",buffer);
    strftime (buffer, sizeof buffer, "The time is %I:%M %p.\n", info);
    printf("%s",buffer);*/

  } else {

  // SET 

    struct timeval now;
    int rc;
    
    now.tv_sec=866208142;
    now.tv_usec=290944;

    rc=settimeofday(&now, NULL);
    if(rc==0) {
      printf("settimeofday() successful.\n");
    }
    else {
      printf("settimeofday() failed, ""errno = %d\n",rc);
    }

  }

  
}

void initialize_time(void) {
    add_cmd("time",cmd_time,HTTPINTERFACE | CONSOLEINTERFACE);
}


/*

time_t now;
char strftime_buf[64];
struct tm timeinfo;

time(&now);
// Set timezone to China Standard Time
setenv("TZ", "CST-8", 1);
tzset();

localtime_r(&now, &timeinfo);
strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);



#include <sys/time.h>
#include <stdio.h>

int main() {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  printf("seconds : %ld\nmicro seconds : %ld",
    current_time.tv_sec, current_time.tv_usec);

  return 0;
}


*/
