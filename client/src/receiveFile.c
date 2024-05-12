#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "net.h"

#define FLPKG_SZ (PACKAGE_SIZE - HEADER_LEN - sizeof(uint16_t))
struct receivePkg {
    uint16_t pkg_cnt;       // 一共要发多少包
    char src_username[32];  // 源用户名
    char filename[32];
};
struct filePkg {
    uint16_t pkg_current;  // 当前是第几个包 (从0开始)
    char data[FLPKG_SZ];   // 剩余部分全为data
};

extern struct curPosition inputZoneStart, inputZoneEnd;
extern char currentUser[32];
extern char currentChat[32];

void receiveFile(char* data) {
    char path[256];
    char filename[256];
    struct receivePkg* package_data = (struct receivePkg*)data;
    sprintf(path, "%s/Chatty/client/user/%s/file/%s.frag", getenv("HOME"), currentUser,
            package_data->filename);
    sprintf(filename, "%s/Chatty/client/user/%s/file/%s", getenv("HOME"), currentUser,
            package_data->filename);
    int pkg_current = 0;

    struct package package;
    if (access(path, F_OK) == 0) {
        // 存在中断文件
        struct stat st;
        if (stat(path, &st) != 0) {
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

    FILE* fd = fopen(path, "a");
    while (1) {
        receveMessage(&package);
        if (package.method == 0) {
            break;
        } else {
            pkg_current++;
            struct filePkg* pp = (struct filePkg*)package.data;
            fwrite(pp->data, sizeof(char), package.length - sizeof(pp->pkg_current), fd);
        }
    }
    if (pkg_current == package_data->pkg_cnt) {
        rename(path, filename);
    }
}