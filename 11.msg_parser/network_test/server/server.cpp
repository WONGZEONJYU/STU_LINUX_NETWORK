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
#include "msg_parser.h"

using namespace std;

int main() 
{
    int server {socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == server){
        cout << "server socket error" << endl;
        return -1;
    }

    sockaddr_in saddr {};

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);//htonl函数把小端转换成大端（网络字节序采用大端）
    saddr.sin_port = htons(8888);

    if ( -1 == bind( server,reinterpret_cast<const sockaddr *>(&saddr),sizeof(saddr) ) ){
        cout << "server bind error" << endl;
        return -1;
    }

    if ( -1 == listen(server,1) ){
        cout << "server listen error" << endl;
        return -1;
    }

    cout << "server start success" << endl;

    MParser * parser {MParser_New()};

    while (true){

        tcp_info info{};
        socklen_t l {sizeof(info)};

        sockaddr_in caddr {};
        socklen_t asize {sizeof(caddr)};

        int client {accept(server,reinterpret_cast<sockaddr *>(&caddr),&asize)};

        if (-1 == client){
            cout << "client accept success" << endl;
            return -1;
        }

        cout << "client :" << client << endl; //client的数值表示系统资源的id

        do{

            getsockopt(client,IPPROTO_TCP,TCP_INFO,&info,&l);

            Message * m { MParser_ReadFd(parser,client) };

            if (m){

                // cout << "type = " << m->type << endl;
                // cout << "cmd = " << m->cmd << endl;
                // cout << "index = " << m->index << endl;
                // cout << "total = " << m->total << endl;
                // cout << "length = " << m->length << endl;
                cout << "payload = " << m->payload << endl;
                free(m);
            }

        } while (TCP_ESTABLISHED == info.tcpi_state);

        cout << "client socket is closed!" << endl;

        close(client);
    }

    close(server);

    return 0;
}

