#include "log.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>

char* logFilePath = "./log";
LogMode logmode = LOGMODE_SYSLOG;
LogLevel loglevel = DEBUG;

FILE* logfd = NULL; 
static const char *log_type_str[] = {NULL, "HTTPD", "V4L2"};
static const char *log_level_str[] = {NULL, "DEBUG", "INFO", "WRNING", "ERROR", "ALL"};

void InitLog(LogMode mode, LogLevel level)
{
	if(mode > 2 || mode < 1)
		logmode = LOGMODE_SYSLOG;
	else
		logmode = mode;
	switch(logmode)
	{
		case LOGMODE_SYSLOG:
		{
			break;
		}
		break;
		case LOGMODE_FILE:
		{
			logfd = fopen(logFilePath, "w+");
			if(NULL == logfd )
			{
				printf("InitLog error\n");
				return;
			}
		}
		break;
		default:
		break;
	}
	
	if(level < 1 || level > 5)
		loglevel = 1;
	else
		loglevel = level;
}


void Log(LogLevel level, LogType type, const char *fmt, ...)
{
	if(!(level <= loglevel || level == ALL))
		return;
	int n;
	char buf[10240];
	n = snprintf(buf, sizeof(buf), "[%s] [%s]", get_log_level_str(level), get_log_type_str(type));

	va_list ap;
    va_start(ap, fmt);
    n += vsnprintf(buf + n, sizeof(buf) - n, fmt, ap);
    va_end(ap);

	switch (logmode) {
	case LOGMODE_FILE:
		strncat(buf, "\n", 10240 - strlen(buf));
		fputs(buf, logfd);
		fflush(logfd);
		break;

	case LOGMODE_SYSLOG:
		strncat(buf, "\n", 10240 - strlen(buf));
		fputs(buf, stderr);
		fflush(stderr);
		break;
	}
}

const char* get_log_level_str(LogLevel level)
{
	if(level > 9 || level < 1)
		return log_level_str[10];
    return log_level_str[level];
}

/**
 * get_log_type_str
 *      Gets string value for type log level.
 *
 * Returns: name of type log level.
 */
const char* get_log_type_str(LogType type)
{
	if(type > 2 || type < 1)
		return log_type_str[3];
    return log_type_str[type];
}