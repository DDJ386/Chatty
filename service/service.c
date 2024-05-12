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
#include <sys/stat.h>

#include "protocol.h"

#define PORT 8848
#define IP "127.0.0.1"
#define BACKLOG 1024
#define FLPKG_SZ (DATA_SEZE - sizeof(uint16_t))

int ServerSocket,ClientSocket;

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
    sprintf(filename,"%s/Chatty/service/user",getenv("HOME"));
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


int ReplytoClient(struct package *packet,int ClientSocket)
{
    size_t len = packet->length + HEADER_LEN;
    printf("reply to socket:%d\n",ClientSocket);
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


int UploadFile(uint8_t *data,int ClientSocket,char *CurrentUser)
{
    printf("func UploadFile\n");
    int pkg_num;
    char DestUser[32], Filename[32];
    struct package reply;
    struct FilePkg 
    {
        uint16_t pkg_current;  // 当前是第几个包 (从0开始)
        char data[FLPKG_SZ];   // 剩余部分全为data
    };

    sscanf(data, "%d%s %s", &pkg_num, DestUser, Filename);

    int current_pkg = 0;
    char frag_file[256];
    char currentname[256];
    sprintf(frag_file,"%s/Chatty/service/%s/Filebox/%s/%s.frag",getenv("HOME"),DestUser, CurrentUser, Filename);
    sprintf(currentname,"%s/Chatty/service/%s/Filebox/%s/%s",getenv("HOME"),DestUser, CurrentUser, Filename);

    if(access(frag_file,F_OK)==0)
    {
        struct stat st;
        if(stat(frag_file,&st)!=0)
        {
            perror("stat error");
            return -1;
        }
        current_pkg=st.st_size/FLPKG_SZ;
        sprintf(reply.data,"%d",current_pkg);
    }
    else
    {
        strcpy(reply.data,"OK");
    }
    reply.method=REPLY;
    reply.length=sizeof(reply.data);
    ReplytoClient((void*)&reply,ClientSocket);

    FILE *frag;
    frag = fopen(frag_file,"a");
    while(1)
    {
        struct package package;
        if(recv(ClientSocket,(void*)&package,sizeof(package),0) < 0L)
        {
            fclose(frag);
            return -1;
        }
        struct FilePkg *pFilePkg = (struct FilePkg *)package.data;
        current_pkg ++;
        if(current_pkg==pkg_num)
        {
            rename(frag_file, currentname);
            fclose(frag);
            return 1;
        }
        else
        {
            fwrite(package.data, sizeof(char), package.length - sizeof(uint16_t),frag);
            fflush(frag);
        }
    }
}

int HandleInquiry(char* CurrentUser,int ClientSocket)
{
    printf("func HandleInquiry\n");
    char buffer[4094]="";
    char data_buffer[4094] ="";
    FILE *fd;
    char msg_path[256];
    char file_path[256];
    char filename[512];
    snprintf(msg_path,256,"%s/Chatty/service/%s/MessageBox",getenv("HOME"), CurrentUser);
    snprintf(file_path,256,"%s/Chatty/service/%s/FileBox",getenv("HOME"),CurrentUser);
    
    struct dirent *dp1,*dp2;
    DIR *dir1=opendir(msg_path);
    DIR *dir2=opendir(file_path);
    if(!dir1) return -1;
    while ((dp1=readdir(dir1))!=NULL)
    {
        int cnt=0;
        if(strcmp(dp1->d_name,".")!=0&&strcmp(dp1->d_name,"..")!=0)
        {
            
            //检查文件与缓冲区的大小
            snprintf(filename, 512,"%s/%s",msg_path,dp1->d_name);
            fd=fopen(filename,"r+");
            char msg[4064];
            while(fscanf(fd,"%[^\n]%*c",msg)!=EOF)
            {
                if(strlen(msg) + strlen(buffer)  + strlen(data_buffer)> sizeof(data_buffer) - 42)
                {
                    
                    sprintf(data_buffer + strlen(data_buffer), "%s %d%s",dp1->d_name,cnt,buffer);
                    struct package reply;
                    reply.method=REPLY;
                    memcpy(reply.data,data_buffer, sizeof(data_buffer));
                    reply.length=strlen(data_buffer);
                    ReplytoClient((void*)&reply,ClientSocket);
                    memset(data_buffer, 0, sizeof(data_buffer));
                    memset(buffer, 0, sizeof(buffer));
                    cnt=0;
                }
                strcat(buffer,msg);
                strcat(buffer,"\n");
                cnt++;
            }
            sprintf(data_buffer + strlen(data_buffer), "%s %d%s",dp1->d_name,cnt,buffer);
            struct package reply;
            reply.method=REPLY;
            strcpy(reply.data,data_buffer);
            reply.length=strlen(data_buffer);
            ReplytoClient((void*)&reply,ClientSocket);
        }
    }
    char cmd[256];
    snprintf(cmd,256,"rm -r /%s/Chatty/service/%s/MessageBox/*",getenv("HOME"),CurrentUser);
    system(cmd);
    closedir(dir1);

    struct FilePkg {
        uint16_t pkg_current;  // 当前是第几个包 (从0开始)
        char data[FLPKG_SZ];   // 剩余部分全为data
    };

    if(!dir2) return -1;
    while((dp2=readdir(dir2))!=NULL)
    {
        if(strcmp(dp2->d_name,".")==0 || strcmp(dp2->d_name,"..")==0)
            continue;
        struct dirent *dp3;
        char file_name[512];
        sprintf(file_name,"%s/Chatty/service/%s/FileBox/%s",getenv("HOME"),CurrentUser,dp2->d_name);
        DIR *dir3=opendir(file_name);
        if(!dir3) return -1;
        while ((dp3=readdir(dir3))!=NULL)
        {
            if(strcmp(dp3->d_name,".")==0 || strcmp(dp3->d_name,"..")==0)
                continue;
            FILE *fd;
            char path[512];
            snprintf(path,512,"%s/Chatty/service/%s/FileBox/%s/%s",getenv("HOME"),CurrentUser,dp2->d_name,dp3->d_name);
            struct stat st;
            if(stat(path,&st) != 0) return -1;
            int pkg_num= (st.st_size-1)/FLPKG_SZ+1;
            struct package reply;
            sprintf(reply.data,"%d%s %s",pkg_num,dp2->d_name,dp3->d_name);
            reply.method=SDFLE;
            reply.length=sizeof(reply.data);
            ReplytoClient((void*)&reply,ClientSocket);

            struct package bufferpkg;
            if(recv(ClientSocket,(void*)&bufferpkg,sizeof(bufferpkg),0) < 0){
                return -1;
            }
            int start = 0;
            if(bufferpkg.method==REPLY&&strcmp(bufferpkg.data,"OK")==0){
            }
            else{
                sscanf(bufferpkg.data,"%d",&start);
            }
            int offset = start * FLPKG_SZ;
            fd=fopen(path,"r");
            fseek(fd, offset, SEEK_SET);
            for(int i = start + 1; i <= pkg_num; i++)
            {
                struct FilePkg *fp = (struct FilePkg *)reply.data;
                if(i == pkg_num)
                    reply.length = (st.st_size % FLPKG_SZ) + sizeof(fp->pkg_current);
                else 
                    reply.length = PACKAGE_SIZE - HEADER_LEN;
                reply.method=SDFLE;
                fread(fp->data, sizeof(char), reply.length - sizeof(fp->pkg_current), fd);
                ReplytoClient((void*)&reply,ClientSocket);
            }            
        }
        closedir(dir3);
    }
    snprintf(cmd,256,"rm -r /%s/Chatty/service/%s/FileBox/*",getenv("HOME"),CurrentUser);
    system(cmd);
    closedir(dir2);
    return 1;
}

void* HandleClient(void* arg)
{
    char CurrentUser[32]="";
    char Filename[32]="";
    int ClientSocket = *(int*)arg;
    free(arg);
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
               ReplytoClient((void *)&reply,ClientSocket);
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
                ReplytoClient((void *)&reply,ClientSocket);
                break;
            }
            case SDMSG: {
                if (SendMessage(data, CurrentUser) != 1) {
                    strcpy(reply.data, "failed");
                } else {
                    strcpy(reply.data, "success");
                }
                ReplytoClient((void *)&reply,ClientSocket);
                break;
            }
            case SDFLE:
            {   

            }
            case INQRY:
            {
                HandleInquiry(CurrentUser, ClientSocket);
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
    int ServerSocket,ClientSocket;
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
        int *para = (int*)malloc(sizeof(int));
        *para = ClientSocket;
        if(pthread_create(&thread, NULL, HandleClient, (void*)para) != 0) {
            perror("Failed to create thread");
            close(ClientSocket);
        }
    }
}