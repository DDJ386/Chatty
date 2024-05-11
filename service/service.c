#include <arpa/inet.h>
#include <dirent.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"

#define PORT 8848
#define IP "127.0.0.1"
#define BACKLOG 1024

int ServerSocket,ClientSocket;

/*int hash(char* user)
{
    int sum=0,N=997;
    for(int i=0;user[i]!='\0';i++)
    {
        sum+=user[i];
    }
    return sum%N;
}*/

int Regis(uint8_t *data)
{
    printf("regis func\n");
    int i=0,sum;
    char username[50];
    char password[50];
    char filename[256];
    sscanf(data,"%s %s", username, password);
    printf("%s\n%s\n", username, password);
    FILE *fd;
    printf("%s\n", filename);
    fd=fopen(filename,"r+");
    char checkUser[32];
    char checkPsd[32];
    while (fscanf(fd, "%s %s", checkUser, checkPsd) != EOF)
    {
        if(strcmp(username, checkUser) == 0) {
            return -1;
        }
    }
    fseek(fd,0L,SEEK_END);
    fprintf(fd,"%s %s\n",username,password);
    fflush(fd);
    char cmd[256];
    sprintf(cmd, "mkdir %s/Chatty/service/%s",getenv("HOME"), username);
    system(cmd);
    sprintf(cmd,"mkdir %s/Chatty/service/%s/%s",getenv("HOME"), username,"MessageBox");
    system(cmd);
    sprintf(cmd,"mkdir %s/Chatty/service/%s/%s",getenv("HOME"), username,"FileBox");
    system(cmd);
    return 1;
}


int LoginCheck(u_int8_t *data, char * CurrentUser)
{
    printf("func login\n");
    char password[32]="";
    char filename[256];
    char User[32]="";

    sscanf(data,"%s %s", User, password);
    printf("username:%s\npassword:%s\n", User, password);
    FILE *fd;
    snprintf(filename, 256, "%s/Chatty/service/user", getenv("HOME"));
    fd=fopen(filename,"r+");
    char checkUser[32];
    char checkPwd[32];
    while(fscanf(fd,"%s %s",checkUser,checkPwd)!=EOF)
    {
        printf("checking username:%s\tpassword:%s\n", checkUser, checkPwd);
        if(strcmp(User,checkUser)==0)
        {
            printf("username matched\n");
            if(strcmp(password,checkPwd)==0)
            {
                printf("password matched\n");
                strcpy(CurrentUser,User);
                return 1;
            }
            else return -1;
        }
    }
    return -1;
}


int ReplytoClient(struct package *packet)
{
    size_t len = packet->length + HEADER_LEN;
    send(ClientSocket, (void *)packet, 4096, 0);
    return 0;
}

int SendMessage(uint8_t *data,char* CurrentUser)
{
    printf("func SendMessage\n");
    int i=0;
    char receiver[32];
    char filename[256];
    char message[4064];
    sscanf(data,"%s %[^\n]%*c",receiver,message);
    printf("dest user:%s\nmessage:%s\n", receiver, message);
    FILE *fd;
    snprintf(filename, 256, "%s/Chatty/service/%s/MessageBox", getenv("HOME"), receiver);
    printf("%s\n", filename);
    fd=fopen(filename,"r+");

    if(access(filename,F_OK) == 0)
    {    
        snprintf(filename, 256, "%s/Chatty/service/%s/MessageBox/%s", getenv("HOME"), receiver,CurrentUser);
        fd=fopen(filename,"a");
        fprintf(fd, "%s\n", message);
        fflush(fd);
        return 1;
    }
    else return -1;
}


// int SendFile(uint8_t *data,int is_first,char* CurrentUser,char* Filename)
// {
//     int i=0;
//     uint16_t pkg_num;
//     char receiver[50];
//     char filename[50];
//     char filen[256];
//     char file[4064];
//     if(is_first)
//     {
//         sscanf(data,"%d %s %s",pkg_num,receiver,filename);
//         FILE *fd;
//         snprintf(filen,256,"%s/Chatty/service/%s/%s/%s",getenv("HOME"),CurrentUser,"Filebox",filename);
//     }
//     else
//     {

//     }
// }

int HandleInquiry(char* CurrentUser)
{
    printf("func HandleInquiry\n");
    char buffer[4064]="";
    char data_buffer[4064] ="";
    FILE *fd;
    char path[256];
    char filename[256];
    snprintf(path,256,"%s/Chatty/service/%s/MessageBox",getenv("HOME"), CurrentUser);
    // printf("%s\n", path);
    struct dirent *dp;
    DIR *dir=opendir(path);
    if(!dir) return -1;
    while ((dp=readdir(dir))!=NULL)
    {
        int cnt=0;
        if(strcmp(dp->d_name,".")!=0&&strcmp(dp->d_name,"..")!=0)
        {
            snprintf(filename, 256,"%s/%s",path,dp->d_name);
            fd=fopen(filename,"r+");
            char msg[4064];
            while(fscanf(fd,"%[^\n]%*c",msg)!=EOF)
            {
                strcat(buffer,msg);
                strcat(buffer,"\n");
                cnt++;
            }
            sprintf(data_buffer + strlen(data_buffer), "%s %d %s",dp->d_name,cnt,buffer);
        }
    }
    char cmd[256];
    snprintf(cmd,256,"rm -r /%s/Chatty/service/%s/MessageBox/*",getenv("HOME"),CurrentUser);
    system(cmd);
    closedir(dir);
    struct package reply;
    reply.method=REPLY;
    strcpy(reply.data,data_buffer);
    reply.length=strlen(data_buffer);
    ReplytoClient((void*)&reply);
    return 1;
}

void* HandleClient(void* arg)
{
    char CurrentUser[32]="";
    char Filename[32]="";
    int ClientSocket = (int )arg;
    struct package buffer,reply;
    int rcv=-1;
    int is_first=1;
    int pkg_cnt=0;
    u_int16_t meth,len;
    u_int8_t data[4064];
    while (1) {
        usleep(1000);
        memset((void *)&buffer, 0, sizeof(buffer));
        memset((void *)&reply, 0, sizeof(reply));
        reply.method = REPLY;
        rcv = recv(ClientSocket, (void *)&buffer, sizeof(buffer), 0);
        meth = buffer.method;
        len = buffer.length;
        if (meth) printf("receive package %d from socket:%d\n", meth, ClientSocket);
        strcpy(data, buffer.data);
        if (rcv < 0) {
            printf("failed to receive package!");
            close(ClientSocket);
            return NULL;
        }
        switch (meth) {
            case REGIS: {
                if (Regis(data) == -1) {
                    strcpy(reply.data, "failed");
                } else {
                    printf("regis success\n");
                    strcpy(reply.data, "success");
                }
                ReplytoClient((void*)&reply);
                close(ClientSocket);
                return NULL;
            }
            case LOGIN: {
                if (LoginCheck(data, CurrentUser) == -1) {
                    strcpy(reply.data, "password incorrect!");
                } else {
                    printf("login success\n");
                    strcpy(reply.data, "success");
                }
                ReplytoClient((void *)&reply);
                break;
            }
            case SDMSG: {
                if (SendMessage(data, CurrentUser) != 1) {
                    strcpy(reply.data, "failed");
                } else {
                    strcpy(reply.data, "success");
                }
                ReplytoClient((void *)&reply);
                break;
            }
            // case SDFLE:
            // {   
            //     if(is_first)
            //     {
                    
            //         pkg_cnt=SendFile(data,1,CurrentUser);
            //         is_first=0;
            //     }
            //     else if(pkg_cnt>0)
            //     {
            //         SendFile(data,0,CurrentUser);
            //         pkg_cnt--;
            //         if(pkg_cnt==0)
            //         {
            //             is_first=1;
            //         }
            //     }
                
            //     break;
            // }
            case INQRY:
            {
                HandleInquiry(CurrentUser);
                break;
            }
            default: {
                break;
            }
        }
    }
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    system("touch user");

    ServerSocket=socket(AF_INET,SOCK_STREAM,0);
    if (ServerSocket == -1) {
        perror("Failed to create socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(ServerSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to bind on port");
        return -1;
    }

    if (listen(ServerSocket, BACKLOG) == -1) {
        perror("Failed to listen");
        return -1;
    }

    printf("start to listen\n");
    // connected
    while (1) {
        ClientSocket = accept(ServerSocket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (ClientSocket == -1) {
            perror("fail to connection");
            continue;
        }
        pthread_t thread;
        if(pthread_create(&thread, NULL, HandleClient, (void*)ClientSocket) != 0) {
            perror("Failed to create thread");
            close(ClientSocket);
        }
    }
}
