#ifndef __SB_MSG_DEF_H__
#define __SB_MSG_DEF_H__

#include <stdio.h>
#include <sys/msg.h>

typedef enum
{
	SB_MOD_FWK = 1,
	SB_MOD_CONTROL,
	SB_MOD_VIEW,
	SB_MOD_ALINK,
	SB_MOD_Reserverd1,
	SB_MOD_Reserverd2,
	SB_MOD_Reserverd3,
	SB_MOD_Reserverd4,
	SB_MOD_Reserverd5,
	SB_MOD_Reserverd6,
	SB_MOD_NUM
} SB_MOD;

typedef enum
{
	SB_CMD_START = 1,
	SB_CMD_RECONNECT,
	SB_CMD_STOP,
	SB_CMD_NUM
} SB_CMD;

typedef enum
{
	SB_STA_INIT =1,
        SB_STA_GET_PWD_DONE,
	SB_STA_GET_PWD_TIMEOUT,
        SB_STA_CONNECT_DONE,
        SB_STA_CONNECT_TIMEOUT,
	SB_STA_DISCONNECT,
	SB_STA_REGISTER_DONE,
	SB_STA_REGISTER_TIMEOUT,
        SB_STA_NUM
} SB_STATE;


typedef struct
{
    int size;
    int source;
    int msg_id;
    int dest;
    char para[1];
} SB_MSG __attribute__((packed));

#define MSG_GET_NO_WAIT			IPC_NOWAIT
#define MSG_GET_WAIT_FOREVER		0

typedef enum
{
    COMMU_TYPE_AWS = 1,
    COMMU_TYPE_ALINK,
    COMMU_TYPE_DATA
}CommuCmdType;

typedef  struct 
{
   int cmdType;  /*1: AWS    2: ALINK    3: DATA*/
   int operType; /*0: OFF    1: ON*/
} CommuReq;

typedef  struct 
{
   int cmdType;  /* 1: AWS    2: ALINK    3: DATA*/
   int operType; /* 0: OFF    1: ON*/
   int result;   /* 0: Success others: failed*/
} CommuRsp;


#endif
