#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/syscall.h>
#include <linux/unistd.h>

#include "sb_msg.h"
#include "sb_msg_api.h"

#define MAX_queue_NUM	100
#define MAX_MSG_LEN		(4096 - sizeof(long) - sizeof(SB_MSG) + 1)

typedef struct{
	long mtype;
	char pmsg[1];
}SYS_MSG __attribute__((packed));

char *modname[] = {
	"none",
	"fwk",
	"control",
	"view",
	"alink",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
};
static int sb_quelist[MAX_queue_NUM] = {0};

int sb_get_queue(int mod_id)
{
	return sb_quelist[mod_id];
}

void sb_remove_queue(void)
{
	int i;
	for (i=0; i<MAX_queue_NUM; i++) {
		msgctl(sb_quelist[i], IPC_RMID, NULL);
	}
}

int sb_create_queue(int mod_id)
{
	int key = mod_id;
	int queue;

	if (mod_id >= SB_MOD_NUM) {
		return -1;
	}

	queue = msgget(key, IPC_CREAT|0666);
	if (queue != -1) {
		sb_quelist[mod_id]	= queue;
	} else {
		loge("sb_create_queue msgget");
	}

	return queue;
}

int sb_send_message(SB_MSG *msg)
{
	SYS_MSG *SysMsg;
	int queue, ret;
	size_t msglen;

	if ((msg == NULL)
			|| (msg->dest < 0 || msg->dest >= SB_MOD_NUM)) {
		logi("sb_msg Invlid message dest=%d\n", msg->dest);
		return -1;
	}
	msglen = sizeof(SYS_MSG) - 1 + msg->size + sizeof(SB_MSG) - 1;
	SysMsg = (SYS_MSG *)SB_MALLOC(msglen);

	SysMsg->mtype = 1;
	memcpy((void *)SysMsg->pmsg, msg, msg->size + sizeof(SB_MSG) - 1);
	queue = sb_quelist[msg->dest];

	if (queue == -1 || queue == 0) {
		logw("Forget to init queue for %s", modname[msg->dest]);
		return -1;
	}

	sb_debug("%s:(s=%s, d=%s, ID=(%x)), SYS_MSG %p, SB_MSG %p", __func__,
			modname[msg->source],
			modname[msg->dest],
			msg->msg_id,
			SysMsg,
			msg);

	ret = msgsnd(queue, SysMsg, msglen - sizeof(long), IPC_NOWAIT);
	if (ret) {
		loge("%s-%d : %s!",
				__FUNCTION__,
				__LINE__,
				strerror(errno));
	}

	SB_FREE(SysMsg);

	return ret;
}

int sb_send_message_ext(int src, int dest,int msg_id, void *data, int size)
{
	int ret = -1;
	SB_MSG *pmsg = (SB_MSG *)SB_MALLOC(sizeof(SB_MSG) + size - 1);

	if (pmsg == NULL) {
		loge("sb_msg malloc error");
		return -1;
	}

	pmsg->source = src;
	pmsg->dest = dest;
	pmsg->msg_id = msg_id;
	pmsg->size = size;
	if (!(size == 0 || data == NULL)) {
		SB_MEMCPY(pmsg->para, size, data, size, size);
	}

	ret = sb_send_message(pmsg);

	SB_FREE(pmsg);

	return ret;
}

SB_MSG* sb_get_message(int queue, int msgflg, int * result)
{

	SYS_MSG *sys_msg;
	int ret;

	sys_msg = (SYS_MSG *)SB_MALLOC(sizeof(SYS_MSG) + MAX_MSG_LEN);
	logi("sb_get_message msg %p sys_msg %p", sys_msg->pmsg, sys_msg);
	SB_MEMSET(sys_msg, 0, sizeof(SYS_MSG) + MAX_MSG_LEN);
	sys_msg->mtype = 1;
	ret = msgrcv(queue, sys_msg, MAX_MSG_LEN, 0,  msgflg);
	if (ret < -1) {
		*result = ret;
		loge("sb_get_message : %s", strerror(errno));
		return NULL;
	}

	*result = ret;

	return (SB_MSG*)sys_msg->pmsg;
}


#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({                      \
		const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		(type *)( (char *)__mptr - offsetof(type,member) );})

int sb_free_message(SB_MSG **msg)
{
	if(*msg == NULL)
		return -1;

	SYS_MSG *smsg = container_of(*msg, SYS_MSG, pmsg);
	logi("sb_free_message msg %p sys_msg %p", *msg, smsg);
	if (smsg)
		SB_FREE(smsg);
	*msg = NULL;
	return 0;
}

