#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "log.h"
#include "sb_msg_def.h"
#include "sb_msg_api.h"

#define ASSERT(__X__, RET_VAL, _FILE_, _LINE_)	do{\
	if((__X__))\
	{\
		loge("\n\nFATAL ERROR:%s:%s at %s:%d\n\n",__FUNCTION__, #__X__, (_FILE_),  (_LINE_));\
		return (RET_VAL);\
	}\
}while(0);

#define ASSERT0(__X__, _FILE_, _LINE_)	do{\
	if((__X__))\
	{\
		loge("\n\nFATAL ERROR:%s:%s at %s:%d\n\n",__FUNCTION__, #__X__, (_FILE_),  (_LINE_));\
	}\
}while(0);

void* SB_Malloc(int size, const char *file, int line)
{
	void* pMem;

	ASSERT((size < 0), NULL, file, line);


	pMem = malloc(size);
	if(pMem == NULL)
	{
		loge("fwk_msg  NO MEMORY!!!!!! file=%s, line=%d\n\n", file, line);
		ASSERT(1, NULL, file, line);
	}
	return pMem;
}

void SB_Free(const char *file, int line, void * mem)
{
	if(mem == NULL)
	{
		loge("fwk_msg Free NULL memory!!!!!! file=%s, line=%d\n\n", file, line);	
		ASSERT0(1, file, line);
		return;
	}

	free(mem);

	return;
}

void *_SB_MEMCPY (void * _DEST_PTR, int _DEST_LEN, void *  _SRC_PTR, int _SRC_LEN, int _SIZE, const char *file, int line)
{
	ASSERT((_DEST_PTR == NULL || _SRC_PTR == NULL), NULL, file, line);
	ASSERT((_DEST_LEN < _SIZE || _SRC_LEN < _SIZE), NULL, file, line);
	return memcpy(_DEST_PTR, _SRC_PTR, _SIZE);
}

void *_SB_MEMSET(void *_DEST_PTR, int _VAL, int _SIZE, const char *file, int line)
{
	ASSERT((_DEST_PTR == NULL), NULL, file, line);
	return memset(_DEST_PTR, _VAL, _SIZE);
}

char * _SB_STRNCPY (char * _DEST_PTR, int  _DEST_LEN, const char *_SRC_PTR, int _SIZE, const char *file, int line)
{
	ASSERT((_DEST_PTR == NULL || _SRC_PTR == NULL), NULL, file, line);
	ASSERT((_DEST_LEN < _SIZE), NULL, file, line);
	SB_MEMSET(_DEST_PTR, 0, _DEST_LEN);
	return strncpy(_DEST_PTR, _SRC_PTR, _SIZE);	
}
#define MAX_SPRINTF_BUFFER_SIZE   (8*1024)
int _SB_SPRINTF (char * _DEST_PTR, int  _DEST_LEN, const char *file, int line, const char *_FMT, ...)
{
	int size;
	char *buffer = NULL;
	va_list ap;

	buffer = (char*)SB_MALLOC(MAX_SPRINTF_BUFFER_SIZE);
	if (buffer == NULL)
		return -1;
	SB_MEMSET(buffer, 0, MAX_SPRINTF_BUFFER_SIZE);
	va_start(ap, _FMT);
	size = vsnprintf(buffer, MAX_SPRINTF_BUFFER_SIZE, _FMT, ap);
	va_end(ap);	
	ASSERT((_DEST_PTR == NULL || _DEST_LEN < SB_STRLEN(buffer)), -1, file, line);
	SB_MEMSET(_DEST_PTR, 0, _DEST_LEN);
	SB_STRNCPY(_DEST_PTR, _DEST_LEN, buffer, SB_STRLEN(buffer));
	SB_FREE(buffer);
	return size;	
}

char *_SB_STRCAT(char *_DEST_PTR, int  _DEST_LEN, const char *_SRC_PTR, const char *file, int line)
{
	ASSERT((_DEST_PTR == NULL || _SRC_PTR == NULL || _DEST_LEN <= SB_STRLEN((char*)_SRC_PTR)), NULL, file, line);
	return strcat(_DEST_PTR, _SRC_PTR);
}
int _SB_STRLEN (char *_STR_PTR, const char *file, int line)
{
	ASSERT0((_STR_PTR == NULL), file, line);	
	return strlen(_STR_PTR);
}
