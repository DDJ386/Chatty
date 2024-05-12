#ifndef _NET_H
#define _NET_H 1
#include "protocol.h"

#ifdef __cplusplus
extern "C" {
#endif
void connectServer();
int sendMessage(struct package *message);
int receveMessage(void *buffer);
void closeConnect();
void netLock();
void netUnlock();
#ifdef __cplusplus
}
#endif

#endif