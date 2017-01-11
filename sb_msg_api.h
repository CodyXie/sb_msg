#ifndef __SB_MSG_API_H__
#define __SB_MSG_API_H__
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

#include "log.h"
#include "sb_msg_def.h"

#define LOG_TAG "sb_msg"


#if 1
#define sb_debug(fmt, ...) \
	do { logd(fmt, ##__VA_ARGS__); } while (0)


#define SB_CHECK( _X_ )	do{ sb_debug(#_X_ "\r\n");\
	if( (_X_)  != 0)\
	{\
		sb_debug( "call %s failed ", #_X_);\
	}\
	sb_debug("Call %s success!", #_X_);\
}while(0)
#endif

extern int sb_get_queue(int mod_id);
extern int sb_create_queue(int mod_id);
extern void sb_remove_queue(void);

extern int sb_send_message(SB_MSG *msg);
extern int sb_send_message_ext(int sour, int dest,int msg_id, void *data, int size);
extern SB_MSG * sb_get_message(int queque, int msgflg, int *ret);
extern int sb_free_message(SB_MSG **msg);

extern void *SB_Malloc(int size, const char * file, int line);
extern void SB_Free(const char *file, int line, void * mem);

extern void *_SB_MEMCPY (void * _DEST_PTR, int _DEST_LEN, void *  _SRC_PTR, int _SRC_LEN, int _SIZE, const char *file, int line);
extern void *_SB_MEMSET(void *_DEST_PTR, int _VAL, int _SIZE, const char *file, int line);
extern char * _SB_STRNCPY (char * _DEST_PTR, int  _DEST_LEN, const char *_SRC_PTR, int _SIZE, const char *file, int line);
extern char *_SB_STRCAT(char *_DEST_PTR, int  _DEST_LEN, const char *_SRC_PTR, const char *file, int line);
extern int _SB_STRLEN (char *_STR_PTR, const char *file, int line);
extern int _SB_SPRINTF (char * _DEST_PTR, int  _DEST_LEN, const char *file, int line, const char *_FMT, ...);


#define SB_MALLOC(size)		SB_Malloc((size), __FILE__, __LINE__)
#define SB_FREE(_PMEM)		do{\
	SB_Free(__FILE__, __LINE__, (_PMEM));  \
	if((_PMEM)) (_PMEM) = NULL;\
}while(0)
#define SB_MEMCPY(_DEST_PTR, _DEST_LEN, _SRC_PTR, _SRC_LEN, _SIZE)  _SB_MEMCPY((_DEST_PTR), (_DEST_LEN), (_SRC_PTR), (_SRC_LEN), (_SIZE), __FILE__, __LINE__)
#define SB_MEMSET(_DEST_PTR, _VAL, _SIZE) _SB_MEMSET((_DEST_PTR), (_VAL), (_SIZE), __FILE__, __LINE__)
#define SB_STRNCPY(_DEST_PTR, _DEST_LEN, _SRC_PTR, _SIZE) _SB_STRNCPY((_DEST_PTR), (_DEST_LEN), (_SRC_PTR), (_SIZE), __FILE__, __LINE__)
#define SB_STRCAT(_DEST_PTR, _DEST_LEN, _SRC_PTR) _SB_STRCAT((_DEST_PTR), (_DEST_LEN), (_SRC_PTR), __FILE__, __LINE__)
#define SB_STRLEN(_STR_PTR) _SB_STRLEN((_STR_PTR), __FILE__, __LINE__)
#define SB_SPRINTF(_DEST_PTR, _DEST_LEN, _FMT, ...) _SB_SPRINTF((_DEST_PTR),(_DEST_LEN), __FILE__,__LINE__,_FMT, ##__VA_ARGS__)

extern int sb_view_init(void);
extern int sb_controller_init(void);

#endif
