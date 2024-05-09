#ifndef _NET_H
#define _NET_H 1

void connectServer();

void sendMessage(struct package *message);

void receveMessage(void *buffer);
#endif