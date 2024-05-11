#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <dirent.h>


#include "protocol.h"


#define PORT 8848
#define IP "127.0.0.1"
#define BACKLOG 1024

int ServerSocket,ClientSocket;
int registed[USER_SIZE];

typedef struct 
{
    char username[50];
    char password[50];
    int state;
}User;

User usertable[USER_SIZE];

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
    int i=0,sum;
    char username[50]="";
    char password[50]="";
    char filename[100];
    while(data[i]!='\0')
    {
        strcat(username,data[i]);
        i++;
    }
    i++;
    username[i]='\0';
    while(data[i]!='\0')
    {
        strcat(password,data[i]);
        i++;
    }
    i++;
    password[i]='\0';

    FILE *fd;
    snprintf(filename, 100, "%s/Chatty/service/%s", getenv("HOME"), "user");
    fd=fopen(filename,'r+');
    char checkUser[32];
    char checkPsd[32];
    while (fscanf(fd, "%s %s", checkUser, checkPsd) != EOF)
    {
        if(strcmp(username, checkUser) == 0) {
            return -1;
        }
    }
    fprintf(fd,"%s %s\n",username,password);
    char cmd[64];
    sprintf(cmd, "mkdir %s/Chatty/service/%s",getenv("HOME"), username);
    system(cmd);
    sprintf(cmd,"mkdir %s/Chatty/service/%s/%s",getenv("HOME"), username,"MessageBox");
    system(cmd);
    sprintf(cmd,"mkdir %s/Chatty/service/%s/%s",getenv("HOME"), username,"FileBox");
    system(cmd);
    return 1;
    //sum=hash(username);
}

int LoginCheck(u_int8_t *data, char * CurrentUser)
{
    int i=0,id;
    char password[50]="";
    char filename[100];
    char User[50]="";
    // while(data[i]!='\0')
    // {
    //     strcat(CurrentUser,data[i]);
    //     i++;
    // }
    // i++;
    // CurrentUser[i]='\0';
    // while(data[i]!='\0')
    // {
    //     strcat(password,data[i]);
    //     i++;
    // }
    // i++;
    // password[i]='\0';
    sscanf(data,"%s %s", User, password);
    FILE *fd;
    snprintf(filename, 100, "%s/Chatty/service/%s", getenv("HOME"), "user");
    fd=fopen(filename,'r+');
    char checkUser[32];
    char checkPwd[32];
    while(fscanf(fd,"%s %s",checkUser,checkPwd)!=EOF)
    {
        if(strcmp(CurrentUser,checkUser)==0)
        {
            if(strcmp(password,checkPwd)==0)
            {
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
    send(ClientSocket, (void*)packet, len, 0);
    return 0;
}

int SendMessage(uint8_t *data,char* CurrentUser)
{
    int i=0;
    char receiver[50];
    char filename[50];
    char message[4064];
    sscanf(data,"%s %s",receiver,message);
    /*while(data[i]!=' ')
    {
        strcat(receiver,data[i]);
        i++;
    }
    i++;
    while(data[i]!=' ')
    {
        strcat(message,data[i]);
        i++;
    }   */
    FILE *fd;
    snprintf(filename, 256, "%s/Chatty/service/%s", "user");
    fd=fopen(filename,'r+');
    char checkUser[32];
    char checkPwd[32];
    int exist=0;
    if(strcmp(CurrentUser,"")==0)
    {
        return -1;
    }
    while(fscanf(fd,"%s %s",checkUser,checkPwd)!=EOF)
    {
        if(strcmp(receiver,checkUser)==0)
        {
            exist=1;
            break;
        }
    }
    if(exist)
    {    
        snprintf(filename, 256, "%s/Chatty/service/%s/%s/%s", getenv("HOME"), receiver,"MessageBox",CurrentUser);
            //filename=getenv("HOME");
            //filename+="/Chatty/service/";
            //filename+=receiver;
            //filename+="/";
            //filename+=sender;
        fd=fopen(filename,'a');
        fprintf(fd, "%s\n", message);
        return 1;
    }
    else return -1;
}

int SendFile(uint8_t *data,int is_first)
{
    int i=0;
    uint16_t pkg_num;
    char receiver[50];
    char filename[50];
    char file[4064];
    if(is_first)
    {
        
    }
    i++;
    while(data[i]!=' ')
    {
        strcat(receiver,data[i]);
        i++;
    }
    i++;
    while(data[i]!=' ')
    {
        strcat(file,data[i]);
        i++;
    }
}

int HandleInquiry(char* CurrentUser)
{
    char data[50][4064];
    int messagecnt[50];
    FILE *fd;
    char path[50];
    char filename[50];
    snprintf(path,50,"%s/Chatty/service/%s/%s",getenv("HOME"), CurrentUser,"MessageBox");
    struct dirent *dp;
    DIR *dir=opendir(path);
    if(!dir) return -1;
    while ((dp=readdir(dir))!=NULL)
    {
        if(strcmp(dp->d_name,".")!=0&&strcmp(dp->d_name,"..")!=0)
        {
            snprintf(filename,"%s/%s",path,dp->d_name);
            fd=fopen(filename,'r+');
            while(fscanf(fd,"%s"))
        }

    }
    
    if(access(filename,F_OK))
}

void* HandleClient(void* arg)
{
    char CurrentUser[32]="";
    int ClientSocket = *(int *)arg;
    free(arg);
    struct package buffer,reply;
    int rcv=-1;
    u_int16_t meth,len;
    u_int8_t data[4064];
    while(1)
    {
        memset((void*)&buffer,0,sizeof(buffer));
        memset((void*)&reply,0,sizeof(reply));
        reply.method=REPLY;
        rcv=recv(ClientSocket,(void*)&buffer,sizeof(buffer), MSG_WAITALL);
        meth=buffer.method;
        len=buffer.length;
        strcpy(data,buffer.data);
        if(rcv<0)
        {
            printf("failed to receive package!");
            close(ClientSocket);
            return NULL;
        }
        switch(meth)
        {
            case REGIS:
            {
                if(Regis(data) == -1)
                {
                    strcpy(reply.data, "failed");
                }
                else
                {
                    strcpy(reply.data, "success");
                }
                ReplytoClient((void*)&reply);
                break;
            }
            case LOGIN:
            {
                if(LoginCheck(data,CurrentUser) == -1)
                {
                    strcpy(reply.data, "password incorrect!");
                }
                else
                {
                    strcpy(reply.data, "success");
                }
                ReplytoClient((void*)&reply);
                break;
            }
            case SDMSG:
            {
                if(SendMessage(data,CurrentUser)!=1)
                {
                    strcpy(reply.data, "failed");
                }
                else
                {
                    strcpy(reply.data, "success");
                }
                ReplytoClient((void*)&reply);
                break;
            }
            case SDFLE:
            {
                    
                SendFile();
            }
            case INQRY:
            {
                HandleInquery();
            }
            default:
            {
                break;
            }
        }
    }
}


int main()
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    
    memset(registed,0,sizeof(registed));
    

    ServerSocket=socket(AF_INET,SOCK_STREAM,0);
    if (ServerSocket == -1) {
        perror("Failed to create socket");
        return -1;
    }

    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(PORT);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    if(bind(ServerSocket,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1)
    {
        perror("Failed to bind on port");
        return -1;
    }

    if(listen(ServerSocket,BACKLOG)==-1)
    {
        perror("Failed to listen");
        return -1;
    }
    
    //connected
    while(1)
    {
        ClientSocket=accept(ServerSocket,(struct sockaddr *)&client_addr,&client_addr_len);
        if(ClientSocket==-1)
        {
            perror("fail to connection");
            continue;
        }
        pthread_t thread;
        if(pthread_create(&thread, NULL, HandleClient, ClientSocket) != 0) {
            perror("Failed to create thread");
            close(ClientSocket);
        }
    }
}
