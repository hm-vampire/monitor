#ifndef LOG_H
#define LOG_H
#include <stdarg.h>
#include <stdio.h>
/* Log levels */
#define ALL		5
#define ERROR		4
#define WARNING		3
#define INFO		2
#define DEBUG		1


/* Log types */
#define LOGHTTPD				1
#define LOGV4L2				2

typedef enum logMode{LOGMODE_FILE = 1, LOGMODE_SYSLOG =2}LogMode;
typedef unsigned int LogLevel; 
typedef unsigned int LogType;

   
	
void InitLog(LogMode type, LogLevel level);
void Log(LogLevel level, LogType type, const char *fmt, ...);
const char* get_log_level_str(LogLevel level);
const char* get_log_type_str(LogType type);

#endif