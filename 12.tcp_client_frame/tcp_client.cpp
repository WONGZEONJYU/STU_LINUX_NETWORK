#include "tcp_client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <cstdlib>
#include "../11.msg_parser/second_edition/msg_parser.h"

struct Client
{
    int fd;
    MParser* parser;
    void* data;
};

TcpClient* TcpClient_New()
{
    return TcpClient_From(-1);
}

TcpClient* TcpClient_From(const int fd)
{
    auto ret {reinterpret_cast<Client *>(malloc(sizeof(Client)))};
    if (ret){
        ret->fd = fd;
        ret->parser = MParser_New();
        ret->data = nullptr;
    }
    return (ret && ret->parser) ? ret : (free(ret),nullptr);
}

int TcpClient_SendMsg(TcpClient* client, Message* msg)
{
    int ret {};
    auto c {reinterpret_cast<Client *>(client)};

    if (c && msg){
        const auto len { Message_Size(msg) };
        auto data { reinterpret_cast<const char*>(Message_H2N(msg)) };
        ret = (-1 != send(c->fd,data,len,0));
        Message_N2H(msg);
    }
    return ret;
}

int TcpClient_SendRaw(TcpClient* client, const char* buf,const int length)
{
    auto c {reinterpret_cast<Client *>(client)};
    return (c && buf) ? send(c->fd,buf,length,0) : 0;
}

Message* TcpClient_RecvMsg(TcpClient* client)
{
    auto c {reinterpret_cast<Client *>(client)};
    return c ? MParser_ReadFd(c->parser,c->fd) : nullptr;
}

int TcpClient_RecvRaw(TcpClient* client, char* buf,const int length)
{
    auto c {reinterpret_cast<Client *>(client)};
    return (c && buf) ? recv(c->fd,buf,length,0): 0;
}

int TcpClient_Connect(TcpClient* client, const char* ip, const int port)
{
    auto ret { TcpClient_IsValid(client) };
    auto c {reinterpret_cast<Client *>(client)};

    if ( (!ret) && ip && c && ( -1 != (c->fd = socket(PF_INET,SOCK_STREAM,0)) ) ) {

        sockaddr_in addr {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(port);
        ret = (-1 != connect( c->fd,reinterpret_cast<sockaddr *>(&addr),sizeof(addr)));
    }
    return ret;
}

int TcpClient_IsValid(TcpClient* client)
{
    int ret{};
    auto c { reinterpret_cast<Client *>(client) };

    if (c){
        tcp_info info{};
        socklen_t info_len { sizeof(info) };
        getsockopt(c->fd, IPPROTO_TCP ,TCP_INFO, &info, &info_len);
        ret = (TCP_ESTABLISHED == info.tcpi_state);
    }
    return ret;
}

void TcpClient_Close(TcpClient* client)
{
    auto c { reinterpret_cast<Client *>(client) };
    if (c){
        close(c->fd);
        c->fd = -1;
        MParser_Reset(c->parser);
    }
}

void TcpClient_Del(TcpClient* client)
{
    auto c {reinterpret_cast<Client *>(client)};
    if (c){
        TcpClient_Close(c);
        MParser_Del(c->parser);
        free(c);
    }
}

void TcpClient_SetData(TcpClient* client, void* data)
{
    auto c { reinterpret_cast<Client *>(client) };
    if (c){
        c->data = data;
    }
}

void* TcpClient_GetData(TcpClient* client)
{
    auto c {reinterpret_cast<Client *>(client)};
    return c ? c->data : nullptr;
}

int TcpClient_Available(TcpClient* client)
{
    auto c {reinterpret_cast<Client *>(client)};
    static char c_temp[1024 * 2] {};
    return c ? recv(c->fd,c_temp,sizeof(c_temp),MSG_PEEK | MSG_DONTWAIT) : -1;
}

int TcpClient_SetOpt(TcpClient* client,int level, int optname, const void* optval, unsigned int optlen)
{
    auto c {reinterpret_cast<Client *>(client)};
    return c ? setsockopt(c->fd,level,optname,optval,optlen) : (-1);
}

int TcpClient_GetOpt(TcpClient* client, int level, int optname, void* optval, unsigned int* optlen)
{
    auto c {reinterpret_cast<Client *>(client)};
    return c ? getsockopt(c->fd,level,optname,optval,optlen) : (-1);
}
