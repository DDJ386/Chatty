#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "protocol.h"


#define PORT 8848
#define IP "127.0.0.1"
#define BACKLOG 1024

int ServerSocket,ClientSocket;

typedef struct 
{
    char username[50];
    char password[50];
    int state;

}User;

User hashtable[USER_SIZE];

int hash(char* user)
{
    int sum=0,N=997;
    for(int i=0;user[i]!='\0';i++)
    {
        sum+=user[i];
    }
    return sum%N;
}

int Regis(uint8_t *data)
{
    int i=0,sum;
    int registed[USER_SIZE];
    memset(registed,0,sizeof(registed));
    char username[50]="";
    char password[50]="";
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
    sum=hash(username);
    if(registed[sum]!=0)
    {
        return -1;
    }
    else
    {
        strcpy(hashtable[sum].username,username);
        strcpy(hashtable[sum].password,password);
        hashtable[sum].state=0;
        registed[sum]=1;
        return 1;
    }
}

int LoginCheck(u_int8_t *data)
{
    int i=0,id;
    char username[50]="";
    char password[50]="";
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
    id=hash(username);
    if(strcmp(hashtable[id].password,password)!=0)
    {
        return -1;
    }
    else
    {
        hashtable[id].state=1;
        return 1;
    }
}

int ReplytoClient(struct package *packet)
{
    size_t len = packet->length + HEADER_LEN;
    send(ClientSocket, (void*)packet, len, 0);
    return 0;
}



int main()
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    struct package buffer,reply;
    int rcv=-1;
    u_int16_t meth,len;
    u_int8_t data[4096];
    

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
    ClientSocket=accept(ServerSocket,(struct sockaddr *)&client_addr,&client_addr_len);
    if(ClientSocket==-1)
    {
        perror("fail to connection");
        return -1;
    }
    //connected
    while(1)
    {
        memset((void*)&buffer,0,sizeof(buffer));
        memset((void*)&reply,0,sizeof(reply));
        reply.method=REPLY;
        rcv=recv(ClientSocket,(void*)&buffer,sizeof(buffer),0);
        meth=buffer.method;
        len=buffer.length;
        strcpy(data,buffer.data);
        if(rcv<0)
        {
            printf("failed to receive package!");
            close(ClientSocket);
            continue;
        }
        if(meth==REGIS)
        {
            if(Regis(data)==-1)
            {
                strcpy(reply.data,"failed");
            }
            else
            {
                strcpy(reply.data,"success");
            }
            ReplytoClient((void*)&reply);
        }
        else if(meth==LOGIN)
        {
            if(LoginCheck(data)==-1)
            {
                strcpy(reply.data,"password incorrect!");
            }
            else
            {
                strcpy(reply.data,"success");
            }
            ReplytoClient((void*)&reply);
        }
        else if(meth==SDMSG)
        {
            SendMessage(data);
        }
    }
}
