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

    for(;;) {

        char input[32]{},buf[128]{};

        cout << "Input: " << endl;

        cin >> input;

        int len ( send(sock,reinterpret_cast<const void *>(input),strlen(input) + 1,0) ) ;

        int r ( recv(sock,buf,sizeof(buf),0) );

        if (r > 0){
            cout << "Receive :" << buf << endl;
        }else{
            cout << "client quit r :" << r << endl;
            break;
        }
    }

    close(sock);

    return 0;
}
