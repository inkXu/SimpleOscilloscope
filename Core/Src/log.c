#include "log.h"

char * EM_LOGLEVELGet(const int level){
	switch (level)
	{
		case LOG_DEBUG:
			return "DEBUG";
		case LOG_INFO:
			return "INFO";
		case LOG_WARN:
			return "WARNING";
		case LOG_ERROR:
			return "ERROR";
		case LOG_FATAL:
			return "FATAL";
		default:
			return "UNKNOWN";
	}
}

void EM_LOG(const int level, const char *func, \
		const int line, const char *fmt, ...){
#if OPEN_ZLOG == 1
	va_list arg;
	va_start(arg, fmt);
	char buf[vsnprintf(NULL, 0, fmt, arg) + 1];
	va_end(arg);

	// I don't know why 'va_start()' needs to be used twice, otherwise %s, %d, %c, etc will error.
	va_start(arg, fmt);
	vsnprintf(buf, sizeof(buf), fmt, arg);
	va_end(arg);

	char result[snprintf(NULL, 0, "[%s] [%s: %d] %s", EM_LOGLEVELGet(level),\
			func, level, buf)];
	sprintf(result, "[%s] [%s: %d] %s", EM_LOGLEVELGet(level),\
			func, level, buf);

#if OPEN_ZLOG_DISPLAY == 1
	if (level >= LOG_LEVEL)
		printf("%s\n", result);
#endif

// TODO save
#if OPEN_ZLOG_SAVE == 1
	SAVELOG(result);
#endif

#endif
}

void savelog(const char *log_message, const char *file_name, const char *mode)
{
#ifdef __linux__
	FILE *file = NULL;

	if((file = fopen(file_name, mode)) == NULL){
		perror("ERROR: ");
		return ;
	}

	char buf[snprintf(NULL, 0, "%s", log_message) + 1];

	int ssize = 0;
	if((ssize = sprintf(buf, "%s\n", log_message)) <= 0){
		fclose(file);
		return ;
	}

	while(fwrite(buf, 1, ssize, file) < 0){
		fclose(file);
		return ;
	}

	fclose(file);
	file = NULL;
#endif
#ifdef __arm__
	// TODO
#endif
#ifdef __x86_64__
	// TODO
#endif
#ifdef __i386__
	// TODO
#endif
}       
