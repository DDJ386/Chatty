#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "net.h"
#include "display.h"

#define FLPKG_SZ (PACKAGE_SIZE - HEADER_LEN - sizeof(uint16_t))

struct filePkg {
    uint16_t pkg_current;  // 当前是第几个包 (从0开始)
    char data[FLPKG_SZ];   // 剩余部分全为data
};

extern struct curPosition inputZoneStart, inputZoneEnd;
extern char currentUser[32];
extern char currentChat[32];

void receiveFile(char* data) {
    char fragFile[256];
    char destFile[256];
    int pkgCnt;
    char srcUsername[32], fileName[32];
    int pkg_current = 0;
    sscanf(data, "%d%s %s", &pkgCnt, srcUsername, fileName);
    sprintf(fragFile, "%s/Chatty/client/user/%s/file/%s.frag", getenv("HOME"), currentUser,
            fileName);
    sprintf(destFile, "%s/Chatty/client/user/%s/file/%s", getenv("HOME"), currentUser, fileName);

    struct package package;
    if (access(fragFile, F_OK) == 0) {
        // 存在中断文件
        struct stat st;
        if (stat(fragFile, &st) != 0) {
            perror("stat error");
            return;
        }
        pkg_current = st.st_size / FLPKG_SZ;
        sprintf(package.data, "%d", pkg_current);
    } else {
        sprintf(package.data, "OK");
    }
    package.method = REPLY;
    package.length = strlen(package.data);
    sendMessage(&package);

    clearInput();
    positionPrint(inputZoneStart, inputZoneEnd, "receiving file: %s from %s please wait\n", fileName, srcUsername);

    FILE* fd = fopen(fragFile, "a");
    while (1) {
        struct filePkg* pp = (struct filePkg*)package.data;
        if (receveMessage(&package) < 0) {
            break;
        } else {
            pkg_current++;
            // printf("receive package %d\n",pkg_current);
            fwrite(pp->data, sizeof(char), package.length - sizeof(pp->pkg_current), fd);
            fflush(fd);
        }
        if (pkg_current == pkgCnt) {
            rename(fragFile, destFile);
            break;
        }
    }
    clearInput();
    // fflush(stdin);
   printf("trans mission over\n");
}