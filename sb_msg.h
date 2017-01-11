#ifndef _SB_MSGID_H__
#define _SB_MSGID_H__

#include "sb_msg_def.h"


#define MSG_ID_SB_FRAMEWORK_START_BASE								(SB_MOD_FWK<<16)

/*control message*/
#define MSG_ID_SB_CONTROL_START_BASE								(SB_MOD_CONTROL<<16)
#define MSG_ID_SB_CONTROL_STA_REQ									(MSG_ID_SB_CONTROL_START_BASE+1)
#define MSG_ID_SB_CONTROL_STA_RSP									(MSG_ID_SB_CONTROL_START_BASE+2)
#define MSG_ID_SB_CONTROL_COMMU_SWITCH_REQ							(MSG_ID_SB_CONTROL_START_BASE+3)
#define MSG_ID_SB_CONTROL_COMMU_SWITCH_RSP							(MSG_ID_SB_CONTROL_START_BASE+4)

/*view message*/
#define MSG_ID_SB_VIEW_START_BASE									(SB_MOD_VIEW<<16)

#endif
