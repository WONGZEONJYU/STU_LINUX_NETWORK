#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <malloc.h>
#include <cstring>
#include "msg_parser.h"
#include "udp_point.h"
struct Point
{
    int fd;
    MParser * parser;
    void * data;
};

static char g_temp[1024 * 4]{};//用于窥探数据长度，没有任何实际意义

static void ParseAddr(struct sockaddr_in addr, char* ip, int* port)
{
    if( ip ){
        strcpy(ip, inet_ntoa(addr.sin_addr));
    }

    if( port ){
        *port = ntohs(addr.sin_port);
    }
}

UdpPoint* UdpPoint_New(int port)
{
    Point * ret { reinterpret_cast<Point * >(malloc(sizeof(Point))) };

    int ok{ !!ret };

    sockaddr_in addr {};

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    ok = ok && (nullptr != (ret->parser = MParser_New()));

    ok = ok && (-1 != (ret->fd = socket(AF_INET,SOCK_DGRAM,0)));

    ok = ok && (-1 != bind(ret->fd,reinterpret_cast<const sockaddr *>(&addr),sizeof(addr)));

    if (ok){

        ret->data = nullptr;
    }else{

        ret ? (MParser_Del(ret->parser),nullptr) : nullptr;
        ret ? close(ret->fd): -1;
        free(ret);
        ret = nullptr;
    }

    return ret;
}

UdpPoint* UdpPoint_From(int fd)
{
    Point * ret { reinterpret_cast<Point * >(malloc(sizeof(Point))) };

    if (ret){

        ret->fd = fd;

        ret->parser = MParser_New();

        ret->data = nullptr;
    }

    return (ret && ret->parser) ? ret : (free(ret),nullptr);
}

int UdpPoint_SendMsg(UdpPoint* point, Message* msg, const char* remote, int port)
{
    Point * c { reinterpret_cast<Point * >(point) } ;

    int ret {};

    if (c && msg && remote){

        int len {Message_Size(msg)};

        char * data {reinterpret_cast<char *>(Message_H2N(msg))};

        ret = UdpPoint_SendRaw(c,data,len,remote,port);

        Message_N2H(msg);
    }

    return ret;
}

int UdpPoint_SendRaw(UdpPoint* point, char* buf, int length, const char* remote, int port)
{
    Point * c { reinterpret_cast<Point * >(point) } ;

    int ret {};

    if (c && buf && remote){

        sockaddr_in addr{};
        socklen_t len {sizeof(addr)};

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(remote);
        addr.sin_port = htons(port);

        ret = (-1 != (sendto(c->fd,buf,length,0,
                    reinterpret_cast<const sockaddr * >(&addr),len)));
    }

    return ret;
}

Message* UdpPoint_RecvMsg(UdpPoint* point, char* remote, int* port)
{
    Point * c { reinterpret_cast<Point * >(point) } ;

    Message * ret{};

    if (c && remote && port){
        
        sockaddr_in raddr {};

        socklen_t addrlen {sizeof(raddr)};

        int length ( recvfrom(c->fd, g_temp, sizeof(g_temp), //窥探一下当前有多少数据
                    MSG_PEEK, reinterpret_cast<sockaddr*>(&raddr), &addrlen) );

        unsigned char* buf {(length > 0) ? reinterpret_cast<unsigned char *>(malloc(length)) : nullptr};

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

int UdpPoint_RecvRaw(UdpPoint* point, char* buf, int length, char* remote, int* port)
{
    Point * c { reinterpret_cast<Point * >(point) };

    int ret {-1};

    if (c && buf && remote && port){

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
    Point * c { reinterpret_cast<Point * >(point) };

    if(c){

        close(c->fd);

        MParser_Del(c->parser);

        free(c);
    }
}

int UdpPoint_Available(UdpPoint* point)
{
    Point * c { reinterpret_cast<Point * >(point) };

    int ret {-1};

    if (c){

        sockaddr_in raddr{};

        socklen_t len {sizeof(raddr)};

        ret = recvfrom(c->fd,g_temp,sizeof(g_temp),(MSG_PEEK | MSG_DONTWAIT),
                        reinterpret_cast<sockaddr *>(&raddr),&len);
    }

    return ret;
}

void UdpPoint_SetData(UdpPoint* point, void* data)
{
    Point * c { reinterpret_cast<Point * >(point) };

    if(c){
        c->data = data;
    }
}

void * UdpPoint_GetData(UdpPoint* point)
{
    Point * c { reinterpret_cast<Point * >(point) };

    void * ret{};

    if(c){
        ret = c->data;
    }

    return ret;
}

int UdpPoint_SetOpt(UdpPoint* point, int level, int optname, const void* optval, unsigned int optlen)
{
    Point * c { reinterpret_cast<Point * >(point) };

    int ret{-1};

    if (c){
        ret = setsockopt(c->fd,level,optname,optval,optlen);
    }

    return ret;
}

int UdpPoint_GetOpt(UdpPoint* point, int level, int optname, void* optval, unsigned int* optlen)
{
    Point * c { reinterpret_cast<Point * >(point) };

    int ret {-1};

    if (c){
        ret = getsockopt(c->fd,level,optname,optval,optlen);
    }

    return ret;
}
