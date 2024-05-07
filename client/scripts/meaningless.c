#include<stdio.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>
#include<stdarg.h>
#include<signal.h>

int main()
{
    struct winsize size;
    printf("\033[2J");
    ioctl(STDIN_FILENO,TIOCGWINSZ,&size);
    printf("col:%d\n",size.ws_col);
    printf("row:%d\n",size.ws_row);
    int pid;
    if((pid = fork()) != 0) {
        char s[256];
        for(int i = 0; i < 10; i++) {
            scanf("%[^\n]",s);
            printf("\033[2J");
            printf("\033[s\033[35;0H%s\033[u",s);
            getchar();
        }
        kill(pid, SIGKILL);
    }
    else {
        int i = 0;
        while(1) {
            printf("\033[s");
            printf("\033[30;%dH",size.ws_col);
            printf("\033[1J");
            printf("\033[H%d",i);
            printf("\033[u");
            fflush(stdout);
            usleep(50000);
            i++;
        }
    }
    return 0;

}
