#ifndef LOGGING_H
#define LOGGING_H
#define _GNU_SOURCE
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h> // uint8_t
#include <stdbool.h> // bool, true, false
#include <string.h> // strerror()
#include <errno.h> // errno
#define BACKTRACE_SIZE 100
#define PATH_MAX 100
FILE *fptr;
int log_start = 0;



 enum {
    INIT_OK = 0,
    INIT_FAILURE = -1
} log_start;

 enum  {
    CLOSE_OK = 0,
    CLOSE_FAILURE = -1
} log_end;

#define INFO "false"
#define WARNING "false"
#define ERROR "true"
//#define DEBUG "true"
#define DEBUG  "false"



#define LOG_ERROR "ERROR "
#define LOG_INFO "INFO "
#define LOG_WARNING "WARNING "
#define LOG_DEBUG "DEBUG "
#define LOG_DEFAULT_MESSAGE "File \"%s\" function %s:%s date_time %s:%s "
#define RETURN_CURSOR_DEFAULT "  \n"
#define RETURN_CURSOR_ERROR " backtrace: \n"

 #define IINFO(x, ...) printf("[INF %s %3d] " x "\r\n", DEBUG, __LINE__, ##__VA_ARGS__);

 void stoplogging(FILE *fptr ){
    if ( fclose(fptr) == EOF) {
        perror("Error closing file");
    } else {
        printf("File closed sucsessfully");
    };
 }

 void  getcallstack(FILE *fp, char* loglevel) {

  if (strstr(loglevel, "ERROR") != NULL) {
    reportTrouble(fp);
  }
  }

 void reportTrouble(FILE *f)
 {
     void *callstack[128];
     int frames = backtrace(callstack, 128);
     char **strs=backtrace_symbols(callstack, frames);
     if (f)
     {
         fprintf(f, " \n");
         for(int i = 0; i < frames; ++i)
         {
             fprintf(f, "%s\n", strs[i]);
         }
     }
     free(f);
 }

 FILE* startlogging(char *filename) {
 printf("Current working dir: %s\n", filename);
 char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
   } else {
       perror("getcwd() error");
   }
   char fullname[300];
  strcpy(fullname, cwd);
  printf("full Current working dir: %s\n", fullname);
   strcat(fullname, "/");
   printf("full Current working dir: %s\n", fullname);
   strcat(fullname, filename);
 printf("full Current working dir: %s\n", fullname);
   // Create a file
 fptr = fopen(fullname, "w");
  if ( fptr  == NULL)  {
      perror ("Unable to start logging\n");
      return NULL;
  }
  else {
      printf("File was opened. Logging started. \n");
      log_start = 0;
        return fptr;
  };
 }



#define TIME_FORMAT "%Y-%m-%d %H:%M:%S" // e.g. 2021-01-04 01:10:38

#define logger(fmt, ...)                                    \
    do {     \
        time_t now = time(NULL);                            \
        char timestr[20];                                   \
        strftime(timestr, 20, TIME_FORMAT, localtime(&now));\
        printf(" \n %s " fmt " \n", timestr, ##__VA_ARGS__);      printf(" \n 111 %s \n", fmt);    \
         FILE *fptr;    fptr = startlogging("log.log2"); fprintf(fptr, "File %s | Line %d :: ", __FILE__, __LINE__);  getcallstack(fptr,fmt); stoplogging(fptr); \
    } while (0)

#define error(fmt, ...) logger("ERROR: %s:%d %s(): " fmt ": %s", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__, strerror(errno))
#define warn(fmt, ...)  logger("WARN: %s:%d %s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define info(fmt, ...)  logger("INFO: %s:%d %s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define debug(fmt, ...)  logger("DEBUG: %s:%d %s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)


#ifdef DEBUG
#define debug(fmt, ...) logger("DEBUG: %s:%d %s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define debug_mem(p, l) memdump(p, l);
#else
#define debug(fmt, ...)
#define debug_mem(p, l)
#endif


void log_addinfo(bool print_backtrace, const char *level_name, const char *format, ...);
#endif // LOGGING_H
