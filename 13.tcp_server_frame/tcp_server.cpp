#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <malloc.h>
#include "tcp_server.h"

#define FD_SIZE FD_SETSIZE

struct Server
{
    int fd;    //服务端socket
    int valid;    //服务端是否有效
    Listener cb;    //回调函数
    TcpClient * client[FD_SIZE];   //存放通信客户端数组
};

TcpServer * TcpServer_New()
{
    Server * ret { reinterpret_cast<Server *>( malloc(sizeof(Server)) ) };
 
    if (ret) {

        ret->fd = -1;

        ret->valid = 0;

        ret->cb = nullptr;
    
        for (int i {}; i < FD_SIZE; i++){

            ret->client[i] = nullptr;
        }
    }

    return ret;
}

int TcpServer_Start(TcpServer * server,int port,int max)
{
    
    Server * s { reinterpret_cast<Server *>(server) };

    if (s && (!s->valid)){
        
        s->fd = socket(PF_INET,SOCK_STREAM,0);

        s->valid = (-1 != s->fd);

        sockaddr_in saddr {};

        saddr.sin_family = AF_INET;

        saddr.sin_addr.s_addr = htonl(INADDR_ANY);                                                              //htonl函数把小端转换成大端（网络字节序采用大端）

        saddr.sin_port = htons(port);

        s->valid = s->valid && bind( s->fd,reinterpret_cast<const sockaddr *>(&saddr),sizeof(saddr) );

        s->valid = s->valid && (-1 != listen(s->fd,1));

    }

    return s->valid;
}

void TcpServer_Stop(TcpServer * server)
{
    Server * s {reinterpret_cast<Server *>(server)};

    if (s){

        s->valid = 0;

        close(s->fd);

        for (int i {}; i < FD_SIZE; i++) {

            TcpClient_Del(s->client[i]);

            s->client[i] = nullptr;
        }

    }
}

void TcpServer_SetListener(TcpServer * server,Listener listener)
{
    Server * s {reinterpret_cast<Server *>(server)};

    if (s){
        s->cb = listener;
    }
    
}

int TcpServer_IsValid(TcpServer * server)
{
    Server * s {reinterpret_cast<Server *>(server)};

    return s ? s->valid : 0;
}

void TcpServer_DoWork(TcpServer * server)
{
    Server * s {reinterpret_cast<Server *>(server)};

    if (s && s->valid){
        
        








    }
}

void TcpServer_Del(TcpServer * server)
{
    TcpServer_Stop(server);

    free(server);
}
