#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "msg_parser.h"
#include "udp_point.h"

struct Point {
    int fd;
    MParser * parser;
    void * data;
};

static void ParseAddr(struct sockaddr_in addr, char* ip, int* port)
{
    if( ip ){
        strcpy(ip, inet_ntoa(addr.sin_addr));
    }

    if( port ){
        *port = ntohs(addr.sin_port);
    }
}

//窥探一下当前有多少数据
static auto has_data_helper(const int fd) 
{
    sockaddr_in raddr {};
    socklen_t addrlen {sizeof(raddr)};
    static char g_temp[1024 * 4]{};
    return recvfrom(fd, g_temp, sizeof(g_temp),
                    MSG_PEEK | MSG_DONTWAIT, 
                    reinterpret_cast<sockaddr*>(&raddr), &addrlen) ;
}

UdpPoint* UdpPoint_New(const int port)
{
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    auto ret { reinterpret_cast<Point * >(malloc(sizeof(Point))) };
    int ok{ !!ret };

    ok = ok && (nullptr != (ret->parser = MParser_New()));

    ok = ok && (-1 != (ret->fd = socket(AF_INET,SOCK_DGRAM,0)));

    ok = ok && (-1 != bind(ret->fd,reinterpret_cast<const sockaddr *>(&addr),sizeof(addr)));

    if (ok){
        ret->data = nullptr;
    }else{
        ret ? (MParser_Del(ret->parser),nullptr) : nullptr; /*通过逗号表达式防止编译报错*/
        ret ? close(ret->fd): -1;
        free(ret);
        ret = nullptr;
    }

    return ret;
}

UdpPoint* UdpPoint_From(const int fd)
{
    auto ret { reinterpret_cast<Point * >(malloc(sizeof(Point))) };

    if (ret){
        ret->fd = fd;
        ret->parser = MParser_New();
        ret->data = nullptr;
    }

    return (ret && ret->parser) ? ret : (free(ret),nullptr);
}

int UdpPoint_SendMsg(UdpPoint* point, Message* msg, const char* remote, int port)
{
    auto c { reinterpret_cast<Point * >(point) } ;

    int ret {};

    if (c && msg && remote){

        const int len {Message_Size(msg)};

        auto data {reinterpret_cast<const char *>(Message_H2N(msg))};

        ret = UdpPoint_SendRaw(c,data,len,remote,port);

        Message_N2H(msg);
    }

    return ret;
}

int UdpPoint_SendRaw(UdpPoint* point, const char* buf,const int length, const char* remote,const int port)
{
    auto c { reinterpret_cast<Point * >(point) } ;

    int ret {};

    if (c && buf && remote){

        sockaddr_in raddr{};
        const socklen_t len {sizeof(raddr)};

        raddr.sin_family = AF_INET;
        raddr.sin_addr.s_addr = inet_addr(remote);
        raddr.sin_port = htons(port);

        ret = (-1 != (sendto(c->fd,buf,length,0,
                    reinterpret_cast<const sockaddr * >(&raddr),len)));
    }

    return ret;
}

Message* UdpPoint_RecvMsg(UdpPoint* point, char* remote, int* port)
{
    auto c { reinterpret_cast<Point * >(point) };
    Message* ret{};

    if (c){

        sockaddr_in raddr {};
        socklen_t addrlen {sizeof(raddr)};

        auto length { has_data_helper(c->fd) };//窥探一下当前有多少数据

        auto buf { (length > 0) ? reinterpret_cast<unsigned char *>(malloc(length)) : nullptr };

        length = recvfrom(c->fd, buf, length, 0, 
                        reinterpret_cast<sockaddr*>(&raddr), &addrlen);

        if (length > 0){
            ret = MParser_ReadMem(c->parser,buf,length);
        }

        if(ret){
            ParseAddr(raddr,remote,port);//解析发送端的 IP地址及端口号
        }
    }

    return ret;
}

int UdpPoint_RecvRaw(UdpPoint* point, char* buf, const int length, char* remote, int* port)
{
    auto c { reinterpret_cast<Point * >(point) };

    int ret {-1};

    if (c && buf){

        sockaddr_in addr{};
        socklen_t len {sizeof(addr)};
        ret = recvfrom(c->fd ,buf ,length ,0 ,reinterpret_cast<sockaddr *>(&addr),&len);

        if (-1 != ret){
            ParseAddr(addr,remote,port);
        }
    }

    return ret;
}

void UdpPoint_Del(UdpPoint* point)
{
    auto c { reinterpret_cast<Point * >(point) };

    if(c){
        close(c->fd);
        MParser_Del(c->parser);
        free(c);
    }
}

int UdpPoint_Available(UdpPoint* point)
{
    auto c { reinterpret_cast<Point * >(point) };
    return c ? has_data_helper(c->fd) : -1;
}

void UdpPoint_SetData(UdpPoint* point, void* data)
{
    auto c { reinterpret_cast<Point * >(point) };
    if(c){
        c->data = data;
    }
}

void* UdpPoint_GetData(UdpPoint* point)
{
    auto c { reinterpret_cast<Point * >(point) };
    return c ? c->data : nullptr;
}

int UdpPoint_SetOpt(UdpPoint* point, int level, int optname, const void* optval, const unsigned int optlen)
{
    auto c { reinterpret_cast<Point * >(point) };
    return c ? setsockopt(c->fd,level,optname,optval,optlen): -1;
}

int UdpPoint_GetOpt(UdpPoint* point, int level, int optname, void* optval, unsigned int* optlen)
{
    auto c { reinterpret_cast<Point * >(point) };
    return c ? getsockopt(c->fd,level,optname,optval,optlen) : -1;
}
