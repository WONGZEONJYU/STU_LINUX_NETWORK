#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "../11.msg_parser/second_edition/msg_parser.h"
#include "tcp_client.h"

using namespace std;

int main(int argc,char* argv[])
{
    const auto server {socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == server){
        cout << "server socket error\n";
        return -1;
    }

    sockaddr_in saddr {};

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);//htonl函数把小端转换成大端（网络字节序采用大端）
    saddr.sin_port = htons(8888);

    if ( -1 == bind( server,reinterpret_cast<const sockaddr *>(&saddr),sizeof(saddr) ) ){
        cout << "server bind error\n";
        return -1;
    }

    if ( -1 == listen(server,1) ){
        cout << "server listen error\n";
        return -1;
    }

    cout << "server start success\n";

    MParser * parser {MParser_New()};

    while (true){

        sockaddr_in caddr {};
        socklen_t asize {sizeof(caddr)};

        const auto client {accept(server,reinterpret_cast<sockaddr *>(&caddr),&asize)};

        if (-1 == client){
            cout << "client accept success\n";
            return -1;
        }

        auto c {TcpClient_From(client)};
        cout << "client :" << client << '\n'; //client的数值表示系统资源的id
        cout << "addr: " << c << '\n';

        do{

            auto m {TcpClient_RecvMsg(c)};

            if (m){
#if 0
                cout << "type = " << m->type << 
                    "\ncmd = " << m->cmd << 
                    "\nindex = " << m->index << 
                    "\ntotal = " << m->total << 
                    "\nlength = " << m->length <<
                    "\npayload = " << m->payload << "\n\n";
#else
                cout << "payload = " << m->payload << '\n';
#endif
                free(m);
            }

        } while (TcpClient_IsValid(c));

        cout << "client socket is closed!\n";

        TcpClient_Del(c);
    }

    close(server);

    return 0;
}

