#include <stdio.h>
//#define PRIVATE_LOG_WRAPPER
#ifndef PRIVATE_LOG_WRAPPER
#include <syslog.h>
#else
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */

#define	LOG_PRIMASK	0x07	/* mask to extract priority part (internal) */
				/* extract priority */
#define	LOG_PRI(p)	((p) & LOG_PRIMASK)
#define	LOG_MAKEPRI(fac, pri)	((fac) | (pri))

#endif

extern void init_log_wrapper(const char *name);
extern void logi(const char *format, ...);
extern void logd(const char *format, ...);
extern void logw(const char *format, ...);
extern void loge(const char *format, ...);
extern void destroy_log_wrapper(void);
