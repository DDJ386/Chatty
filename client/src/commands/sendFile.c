#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "display.h"
#include "net.h"

#define FLPKG_SZ (DATA_SEZE - sizeof(uint16_t))

struct firstPkg {
    uint16_t pkg_cnt;       // 一共要发多少包
    char dst_username[32];  // 目的用户名
    char filename[256];
};

struct filePkg {
    uint16_t pkg_current;  // 当前是第几个包 (从0开始)
    char data[FLPKG_SZ];        // 剩余部分全为data
};

extern struct curPosition inputZoneStart, inputZoneEnd;
extern char currentChat[32];
extern char currentUser[32];

void sendFile() {
    // open file
    char filename[256];
    char path[512];
    scanf(" %s", filename);
    sprintf(path, "%s/Chatty/client/user/%s/file/%s", getenv("HOME"), currentUser, filename);

    FILE *fd;
    fd = fopen(path, "r");
    if (fd == NULL) {
        positionPrint(inputZoneStart, inputZoneEnd, "%s does not exist\n", path);
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        perror("stat error");
        return ;
    }

    // send first package
    struct package package;
    struct firstPkg *fp = (struct firstPkg *)package.data;
    fp->pkg_cnt = (st.st_size - 1) / FLPKG_SZ + 1;
    snprintf(fp->dst_username, 32, "%s", currentChat);
    snprintf(fp->filename, 256, "%s", filename);
    netLock();
    sendMessage(&package);
    receveMessage(&package);
    netUnlock();

    // send file
    if (package.method == REPLY) {
        int start = 0;
        if (strcmp(package.data, "OK") == 0) {
        } else {
            // 断点续传, 设置start
            sscanf(package.data, "%d", &start);
            int offset = start * FLPKG_SZ;
            fseek(fd, offset, SEEK_SET);
        }
        // 传输文件
        netLock();
        for (int i = start; i < fp->pkg_cnt; i++) {
            struct filePkg *pp = (struct filePkg *)package.data;
            package.method = SDFLE;
            if (i == fp->pkg_cnt - 1)
                package.length = st.st_size % FLPKG_SZ + sizeof(pp->pkg_current);
            else
                package.length = PACKAGE_SIZE - HEADER_LEN;
            fread(pp->data, sizeof(char), package.length - sizeof(pp->pkg_current), fd);
            sendMessage(&package);
        }
        netUnlock();
    }
}