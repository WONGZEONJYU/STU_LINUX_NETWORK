#include "tcp_client.h"
#include <malloc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include "../11.msg_parser/msg_parser.h"

struct Client
{
    int fd;
    MParser * parser;
    void * data;
};

TcpClient* TcpClient_New()
{
    return TcpClient_From(-1);
}

TcpClient* TcpClient_From(int fd)
{
    Client * ret {reinterpret_cast<Client *>(malloc(sizeof(Client)))};

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

    Client * c {reinterpret_cast<Client *>(client)};

    if (c && msg){

        int len { Message_Size(msg) };

        char *data { reinterpret_cast<char *>(Message_H2N(msg)) };

        ret = (-1 != send(c->fd,data,len,0));

        Message_N2H(msg);
    }

    return ret;
}

int TcpClient_SendRaw(TcpClient* client, char* buf, int length)
{
    int ret {};

    Client * c {reinterpret_cast<Client *>(client)};

    if (c && buf){

        ret = send(c->fd, buf,length,0);
    }

    return ret;
}

Message* TcpClient_RecvMsg(TcpClient* client)
{
    Message * ret {};

    Client * c {reinterpret_cast<Client *>(client)};

    if (c){

        ret = MParser_ReadFd(c->parser,c->fd);
    }

    return ret;
}

int TcpClient_RecvRaw(TcpClient* client, char* buf, int length)
{
    int ret {};

    Client * c {reinterpret_cast<Client *>(client)};

    if (c && buf){

        ret = recv(c->fd,buf,length,0);
    }

    return ret;
}

int TcpClient_Connect(TcpClient* client, char* ip, int port)
{
    int ret {TcpClient_IsValid(client)};

    Client * c {reinterpret_cast<Client *>(client)};
    
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

    Client * c {reinterpret_cast<Client *>(client)};

    if (c){

        tcp_info info{};

        socklen_t l {sizeof(info)};

        getsockopt(c->fd, IPPROTO_TCP ,TCP_INFO, &info, &l);

        ret = (TCP_ESTABLISHED == info.tcpi_state);
    }

    return ret;
}

void TcpClient_Close(TcpClient* client)
{
    Client * c {reinterpret_cast<Client *>(client)};

    if (c){

        close(c->fd);

        c->fd = -1;

        MParser_Reset(c->parser);
    }
}

void TcoClient_Del(TcpClient* client)
{
    Client * c {reinterpret_cast<Client *>(client)};

    if (c){

        TcpClient_Close(c);

        MParser_Del(c->parser);

        free(c);
    }
}

void TcpClient_SetData(TcpClient* client, void* data)
{
    Client * c { reinterpret_cast<Client *>(client) };
    
    if (c){

        c->data = data;
    }
}

void* TcpClient_GetData(TcpClient* client)
{
    void *ret {};

    Client * c {reinterpret_cast<Client *>(client)};

    if (c){

        ret = c->data;
    }

    return ret;
}
