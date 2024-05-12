#ifndef _PROTOCOL_H
#define _PROTOCOL_H 1
#include <stdint.h>

#define REGIS 1
#define LOGIN 2
#define SDMSG 3
#define SDFLE 4
#define FOLLW 5
#define REPLY 6
#define INQRY 7
#define LOGOUT 8

#define HEADER_LEN (2*sizeof(uint16_t))
#define PACKAGE_SIZE sizeof(struct package)
#define DATA_SEZE (PACKAGE_SIZE - HEADER_LEN)
struct package {
    uint16_t method;
    uint16_t length;
    char data[4064];
};

#endif