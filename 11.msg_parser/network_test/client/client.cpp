#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "message.h"

using namespace std;

static void hton(Message * m)
{
    m->type = htons(m->type);
    m->cmd = htons(m->cmd);
    m->index = htons(m->index);
    m->total = htons(m->total);
    m->length = htonl(m->length);
}

int main() 
{
    int sock{socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == sock){
        cout << "socket error" << endl;
        return -1;
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8888);

    if ( -1 == connect( sock,reinterpret_cast<sockaddr *>(&addr),sizeof(addr) )){
        cout << "connect error" << endl;
        return -1;
    }

    cout << "connect success sock :" << sock << endl;

    const char * test {"HelloWorld"};

    for (int i {}; i < strlen(test); i++){

        char buf[2]{};

        buf[0] = test[i];

        Message *pm { Message_New(128,129,i,strlen(test),buf,2) };

        hton(pm);

        send(sock,pm,(sizeof(Message) + 2),0);

        //usleep(1000);

        free(pm);
    }

    getchar();

    close(sock);

    return 0;
}
