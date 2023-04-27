#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "message.h"
#include "tcp_client.h"

using namespace std;

int main() 
{
    TcpClient * client {TcpClient_New()};

    if (client && TcpClient_Connect(client,"127.0.0.1",8888)){

        const char * test {"wongzeonjyu"};

        for (int i {}; i < strlen(test); i++){

            char buf[2]{};

            buf[0] = test[i];

            Message *pm { Message_New(128,129,i,strlen(test),buf,2) };

            TcpClient_SendMsg(client,pm);

            free(pm);
        }

        getchar();
    }

    TcpClient_Del(client);

    return 0;
}
