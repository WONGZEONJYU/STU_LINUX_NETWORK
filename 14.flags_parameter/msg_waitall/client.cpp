#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    int sock{socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == sock){
        cout << "socket error\n";
        return -1;
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8888);

    if ( -1 == connect( sock,reinterpret_cast<sockaddr *>(&addr),sizeof(addr) )){
        cout << "connect error\n";
        return -1;
    }

    cout << "connect success sock :" << sock << '\n';

    const char * test {"helloworld"};

    for (int i {}; i < strlen(test); i++){
        send(sock,(test + i),1,0);
        if (i % 2){
            sleep(1);
        }
    }

    test = "quit";

    int i {};
    for (; i < (strlen(test) - 1); i++){
        send(sock,(test + i),1,MSG_MORE);
        sleep(1);
    }

    send(sock,test + i,1,0);
    getchar();
    close(sock);

    return 0;
}
