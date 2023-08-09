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
    int sock {socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == sock){
        cout << "socket error\n";
        return -1;
    }

    // 47.99.217.175
    sockaddr_in addr {};

    addr.sin_family = AF_INET;

    //addr.sin_addr.s_addr = inet_addr("47.99.217.175");
    /*inet_addr() 将有效字符串转换为32位二进制网络字节序的IPV4地址*/

    if (!inet_aton("47.99.217.175",&addr.sin_addr)){
        cout <<  "changer error\n";
        return -1;
    }

    addr.sin_port = htons(80);
    /*htons() 把本机字节序转换成网络字节序*/

    if (-1 == connect(sock,reinterpret_cast<sockaddr *>(&addr),sizeof(addr)) ){
        cout << "connect error\n";
        return -1;
    }

    cout << "connect success\n";

    constexpr char to_send[] {"GET /index.html HTTP/1.1\nHOST:www.dt4sw.com\nUser-Agent: TEST\nConnection:close\n\n"};

    int len ( send(sock,to_send,strlen(to_send),0) );

    cout << "send bytes :" << len << '\n';

    len = 0;

    int r{};

    do{   
        char buf[128]{};

        r = recv(sock,buf,sizeof(buf)/sizeof(*buf),0);

        if (r > 0){
            len += r;
        }

        for (int i {}; i < r; i++){
            cout << buf[i];
        }

    } while (r > 0);

    cout << "\nrecv len = " << len << '\n';

    close(sock);

    return 0;
}
