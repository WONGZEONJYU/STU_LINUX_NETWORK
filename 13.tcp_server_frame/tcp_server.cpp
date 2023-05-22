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

    if (s && (!s->valid)) {

        s->fd = socket(PF_INET,SOCK_STREAM,0);

        s->valid = (-1 != s->fd);

        sockaddr_in saddr {};

        saddr.sin_family = AF_INET;

        saddr.sin_addr.s_addr = htonl(INADDR_ANY); //htonl函数把小端转换成大端（网络字节序采用大端）

        saddr.sin_port = htons(port);

        s->valid = s->valid && (-1 != bind( s->fd,reinterpret_cast<const sockaddr *>(&saddr),sizeof(saddr) ));

        s->valid = s->valid && (-1 != listen(s->fd,max));

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

static int SelectHandler(Server  *s ,
                        fd_set * rest,fd_set * reads,
                        int num,int max)
{
    int ret {max};

    //for (int i {0}; i <= max; i++){
    for (int i {s->fd}; i <= max; i++){

        if (FD_ISSET(i,rest)){

            int index {i},event {-1};

            if (index == s->fd){

                sockaddr_in addr{};

                socklen_t asize {sizeof(addr)};

                index = accept(s->fd,reinterpret_cast<sockaddr *>(&addr),&asize);

                if (index > -1) {

                    FD_SET(index,reads);

                    ret = (index > max ) ? index : max;

                    s->client[index] = TcpClient_From(index);

                    event = EVT_CONN;
                }

            }else{
                event = EVT_DATA;
            }

            if (s->cb){

                if (TcpClient_IsValid(s->client[index])){

                    s->cb(s->client[index],event);
                }else{

                    if (s->client[index]){

                        s->cb(s->client[index],EVT_CLOSE);
                    }

                    TcpClient_Del(s->client[index]);

                    s->client[index] = nullptr;

                    FD_CLR(index,reads);

                }
            }
        }
    }

    return ret;
}

void TcpServer_DoWork(TcpServer * server)
{
    Server * s {reinterpret_cast<Server *>(server)};

    if (s && s->valid){

        int max{s->fd};

        fd_set reads{};

        FD_ZERO(&reads);
        FD_SET(s->fd,&reads);

        while( s->valid ) {

            fd_set rset { reads };

            timeval timeout{ .tv_sec = 0,.tv_usec = 10000 };

            int num{ select((max + 1),&rset,nullptr,nullptr,&timeout) };

            if (num > 0){
                
                max = SelectHandler(s, &rset, &reads, num, max);
            }
        }
    }
}

TcpClient* TcpServer_Accept(TcpServer* server)
{
    Server * s {reinterpret_cast<Server *>(server)};

    TcpClient * ret {};

    if (s){

        sockaddr_in caddr{};

        socklen_t asize {sizeof(caddr)};

        int fd {accept(s->fd,reinterpret_cast<sockaddr*>(&caddr),&asize)};

        if (fd > -1){

            ret = TcpClient_From(fd);

            if (!ret){
                close(fd);
            }
        }
    }

    return ret;
}

void TcpServer_Del(TcpServer * server)
{
    TcpServer_Stop(server);

    free(server);
}

int TcpServer_SetOpt(TcpServer * server,int level, int optname, const void* optval, unsigned int optlen)
{
    Server * s {reinterpret_cast<Server *>(server)};

    int ret{-1};

    if (s){
        ret = setsockopt(s->fd,level,optname,optval,optlen);
    }

    return ret;
}

int TcpServer_GetOpt(TcpServer * server, int level, int optname, void* optval, unsigned int* optlen)
{
    Server * s {reinterpret_cast<Server *>(server)};

    int ret {-1};

    if (s){
        ret = getsockopt(s->fd,level,optname,optval,optlen);
    }

    return ret;
}

