#ifndef __JANUS_H__
#define __JANUS_H__

#include <stdint.h>
#include "submoduleComms.h"

typedef uint8_t fnr_t;
#define FNR_NEUTRAL ((fnr_t)0u)
#define FNR_FORWARD ((fnr_t)1u)
#define FNR_REVERSE ((fnr_t)2u)
#define FNR_INVALID ((fnr_t)3u)

typedef uint8_t janusMsg_t;
#define JANUS_STATUS  ((janusMsg_t)0u) 
#define JANUS_SET_FNR ((janusMsg_t)1u)
#define JANUS_GET_FNR ((janusMsg_t)2u)

commsStatus_t getJanusStatus();
fnr_t getFNR();
void setFNR(fnr_t state);
#endif