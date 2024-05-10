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

#define HEADER_LEN 32
#define PACKAGE_SIZE 4096
#define USER_SIZE 1000
struct package {
  uint16_t method;
  uint16_t length;
  uint8_t data[4064];
}

#endif