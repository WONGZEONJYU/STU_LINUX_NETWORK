#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

int main() 
{
    int sock{socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == sock)
    {
        cout << "socket error" << endl;
        return -1;
    }

    // 47.99.217.175
    sockaddr_in addr {};

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("47.99.217.175");
    addr.sin_port = htons(80);

    if (connect(sock,reinterpret_cast<sockaddr *>(&addr),sizeof(addr)) == -1)
    {
        cout << "connect error" << endl;
        return -1;
    }

    cout << "connect success" << endl;

    const char * to_send {"GET /index.html HTTP/1.1\nHOST:www.dt4sw.com\nUser-Agent: TEST\nConnection:close\n\n"};

    int len ( send(sock,reinterpret_cast<const void *>(to_send),strlen(to_send),0) );

    cout << "send bytes :" << len << endl;

    len = 0;

    int r{};

    do
    {   
        char buf[128]{};

        r = recv(sock,reinterpret_cast<void *>(buf),sizeof(buf)/sizeof(char),0);

        if (r > 0){
            len += r;
        }

        for (int i {}; i < r; i++){
            cout << buf[i];
        }

    } while (r > 0);
    
    cout << endl;

    cout << "recv len = " << len << endl;

    close(sock);

    return 0;
}
