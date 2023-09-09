#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    int server {socket(PF_INET,SOCK_DGRAM,0)};

    if (-1 == server){
        cout << "server socket error" << endl;
        return -1;
    }

    sockaddr_in saddr {};
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);//htonl函数把小端转换成大端（网络字节序采用大端）
    saddr.sin_port = htons(8888);

    if ( -1 == bind( server,reinterpret_cast<const sockaddr *>(&saddr),sizeof(saddr) ) ){
        cout << "udp server bind error\n";
        return -1;
    }

    cout << "udp server start success\n";

    for(;;){

        sockaddr_in remote {};
        socklen_t len {sizeof(remote)};
        char buf[128]{};
        const auto r {recvfrom(server,buf,sizeof(buf),0,reinterpret_cast<sockaddr* >(&remote),&len)};

        buf[r] = 0;

        cout << "r = " << r << 
            "\nbuf = " << buf << 
            "\nremote ip = " << inet_ntoa(remote.sin_addr) << 
            "\nremote port = " << ntohs(remote.sin_port) << '\n';

        sendto(server,buf,r,0,reinterpret_cast<const sockaddr * >(&remote),len);
    }

    close(server);
    return 0;
}
