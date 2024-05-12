#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "display.h"
#include "net.h"

#define FLPKG_SZ (DATA_SEZE - sizeof(uint16_t))

struct filePkg {
    uint16_t pkg_current;     // 当前是第几个包 (从0开始)
    char fileData[FLPKG_SZ];  // 剩余部分全为data
};

extern struct curPosition inputZoneStart, inputZoneEnd;
extern char currentChat[32];
extern char currentUser[32];

FILE *fd;
int pkg_num;
struct stat st;

void sendFile() {
    // open file
    char filename[50];
    char path[256];
    scanf(" %s", filename);
    sprintf(path, "%s/Chatty/client/user/%s/file/%s", getenv("HOME"), currentUser, filename);

    
    fd = fopen(path, "r");
    if (fd == NULL) {
        positionPrint(inputZoneStart, inputZoneEnd, "%s does not exist\n", path);
    }

    
    if (stat(path, &st) != 0) {
        perror("stat error");
        return;
    }

    // send first package
    clearInput();
    positionPrint(inputZoneStart, inputZoneEnd,"sending file %s to %s",filename, currentChat);
    struct package package;
    package.method = SDFLE;
    pkg_num = (st.st_size - 1) / FLPKG_SZ + 1;
    sprintf(package.data, "%d%s %s", pkg_num, currentChat, filename);
    netLock();
    sendMessage(&package);
    netUnlock();
}

void startToSend(char* data) {
    // send file
    int start = 0;
    if (strcmp(data, "OK") == 0) {
    } else {
        // 断点续传, 设置start
        sscanf(data, "%d", &start);
        int offset = start * FLPKG_SZ;
        fseek(fd, offset, SEEK_SET);
    }
    // 传输文件
    netLock();
    for (int i = start + 1; i <= pkg_num; i++) {
        struct package package;
        struct filePkg *pp = (struct filePkg *)(package.data);
        package.method = SDFLE;
        if (i == pkg_num)
            package.length = (st.st_size % FLPKG_SZ) + sizeof(pp->pkg_current);
        else
            package.length = PACKAGE_SIZE - HEADER_LEN;
        pp->pkg_current = i;
        fread(pp->fileData, sizeof(char), package.length - sizeof(pp->pkg_current), fd);
        sendMessage(&package);
    }
    netUnlock();
    fclose(fd);
    clearInput();
    // fflush(stdin);   
    printf("trans mission over\n");
}