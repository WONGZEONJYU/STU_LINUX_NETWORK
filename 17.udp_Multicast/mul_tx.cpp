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
    const int server {socket(PF_INET,SOCK_DGRAM,0)};

    if (-1 == server){
        cout << "tx socket error\n";
        return -1;
    }

    sockaddr_in local {};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);//htonl函数把小端转换成大端（网络字节序采用大端）
    //local.sin_addr.s_addr = inet_addr("192.168.80.128");
    local.sin_port = htons(8888);

    if ( -1 == bind( server,reinterpret_cast<const sockaddr *>(&local),sizeof(local) ) ){
        cout << "tx bind error\n";
        return -1;
    }

    cout << "tx start success\n";

    socklen_t len{};

#if 1

    {
        int ttl{};
        len  = sizeof(ttl);
        getsockopt(server,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,&len);
        cout << "default ttl = " << ttl << '\n';

        //设置TTL的值，路由器转发的次数
        ttl = 32;
        len  = sizeof(ttl);
        setsockopt(server,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,len);
        cout << "current ttl = " << ttl << '\n';
    }

    {
        int loop{};//数据是否回环发送回本机 0不发送，非0发送 ,默认非0
        len = sizeof(loop);
        getsockopt(server,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,&len);
        cout << "default loop = " << loop << '\n';
#if 0
        /*把loop设置为0,本地不接收回环数据*/
        loop = 0;
        len = sizeof(loop);
        setsockopt(server,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,len);
        cout << "current loop = " << loop << '\n';
#endif
    }

    {    
        in_addr addr;
#if 1
        len = sizeof(addr);
        getsockopt(server,IPPROTO_IP,IP_MULTICAST_IF,&addr,&len);
        cout << "get ip = " << inet_ntoa(addr) << '\n';
#endif
        addr.s_addr = inet_addr("10.10.10.126");//采用那个网卡作为多播的发送网卡
        //addr.s_addr = htonl(INADDR_ANY);//多个网卡不建议采用0.0.0.0这个地址，有可能出现接收端无法收到数据的情况
        len = sizeof(addr);
        setsockopt(server,IPPROTO_IP,IP_MULTICAST_IF,&addr,len);
        cout << "set ip = " << inet_ntoa(addr) << '\n';
    }

#endif

    sockaddr_in remote {};
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = inet_addr("224.1.1.168");/*多播IP地址*/
    remote.sin_port = htons(9000);
    len = sizeof(remote);

    char buf[32]{"hello_world"};
    const auto r {strlen(buf)};

    for(;;){
        sendto(server,buf,r,0,reinterpret_cast<const sockaddr * >(&remote),len);
        sleep(1);
    }

    close(server);

    return 0;
}
