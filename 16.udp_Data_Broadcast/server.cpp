#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    int server {socket(PF_INET,SOCK_DGRAM,0)};

    if (-1 == server){
        cout << "server socket error\n";
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

    sockaddr_in remote {};

    remote.sin_family = AF_INET;
    //remote.sin_addr.s_addr = inet_addr("10.211.55.255");  //直接广播，需知道当前网络标识
    remote.sin_addr.s_addr = 0xffffffff;                    //本地广播，不经过路由器
    remote.sin_port = htons(9000);
    socklen_t len {sizeof(remote)};

    int brd {1};
    setsockopt(server,SOL_SOCKET,SO_BROADCAST,&brd,sizeof(brd));//socket广播设置
    /*如果没有设置socket广播或者brd设置为0 , 是不会进行广播的*/
    char buf[32]{"hello_world"};
    const auto r { strlen(buf)} ;
    buf[r] = 0;

    for(;;){
        sendto(server,buf,r,0,reinterpret_cast<const sockaddr * >(&remote),len);
        sleep(1);
    }

    close(server);
    return 0;
}
