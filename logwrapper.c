#include <stdio.h>
#include <stdarg.h>

#include "log.h"

void init_log_wrapper(const char *name)
{
#ifndef PRIVATE_LOG_WRAPPER
	openlog(name, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);
#endif
}

void logi(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
#ifndef PRIVATE_LOG_WRAPPER
	vsyslog(LOG_INFO, format, ap);
#else
	fprintf(stderr, "[I] ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");
#endif
	va_end(ap);
}

void logd(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
#ifndef PRIVATE_LOG_WRAPPER
	vsyslog(LOG_DEBUG, format, ap);
#else
	fprintf(stderr, "[D] ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");
#endif
	va_end(ap);
}

void logw(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
#ifndef PRIVATE_LOG_WRAPPER
	vsyslog(LOG_WARNING, format, ap);
#else
	fprintf(stderr, "[W] ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");
#endif
	va_end(ap);
}

void loge(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
#ifndef PRIVATE_LOG_WRAPPER
	vsyslog(LOG_ERR, format, ap);
#else
	fprintf(stderr, "[E] ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");
#endif
	va_end(ap);
}


void destroy_log_wrapper(void)
{
#ifndef PRIVATE_LOG_WRAPPER
	closelog();
#endif
}
