#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc,char* argv[])
{
    int sock{socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == sock){
        cout << "socket error\n";
        return -1;
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    //addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (!inet_aton("127.0.0.1",&addr.sin_addr)){
        cout << "changer error\n";
        return -1;
    }

    /*此处ip是服务端ip,由于是在本机做实验,我选用环回地址*/
    addr.sin_port = htons(8888);

    if ( -1 == connect( sock,reinterpret_cast<sockaddr *>(&addr),sizeof(addr) )){
        cout << "connect error\n";
        return -1;
    }

    cout << "connect success sock :" << sock << '\n';

    for(;;) {

        char input[32]{},buf[128]{};

        cout << "Input: \n";

        cin >> input;

        int len ( send(sock,input,(strlen(input) + 1),0) ) ;

        int r ( recv(sock,buf,sizeof(buf),0) );

        if (r > 0){
            cout << "Receive :" << buf << '\n';
        }else{  /* 与服务端断开连接 recv(...) 返回 0 */
            cout << "client quit r :" << r << '\n';
            break;
        }
    }

    close(sock);

    return 0;
}
