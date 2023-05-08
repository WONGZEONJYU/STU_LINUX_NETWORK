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
    int server {socket(PF_INET,SOCK_DGRAM,0)};

    if (-1 == server){
        cout << "tx socket error" << endl;
        return -1;
    }

    sockaddr_in local {};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);//htonl函数把小端转换成大端（网络字节序采用大端）
    //local.sin_addr.s_addr = inet_addr("192.168.80.128");
    local.sin_port = htons(8888);

    if ( -1 == bind( server,reinterpret_cast<const sockaddr *>(&local),sizeof(local) ) ){
        cout << "tx bind error" << endl;
        return -1;
    }

    cout << "tx start success" << endl;

    socklen_t len{};

    {
        int ttl{};
        len  = sizeof(ttl);
        getsockopt(server,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,&len);
        cout << "default ttl = " << ttl << endl;

        //设在TTL的值，路由器转发的次数
        ttl = 32;
        len  = sizeof(ttl);
        setsockopt(server,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,len);
        cout << "current ttl = " << ttl << endl;
    }

    {
        int loop{};//数据是否回环发送回本机 0不发送，非0发送
        len = sizeof(loop);
        getsockopt(server,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,&len);
        cout << "default loop = " << loop << endl;
        
        // loop = 0;
        // len = sizeof(loop);
        // setsockopt(server,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,len);
        // cout << "current loop = " << loop << endl;
    }

    {    
        in_addr addr;
        // len = sizeof(addr);
        // getsockopt(server,IPPROTO_IP,IP_MULTICAST_IF,&addr,&len);
        // cout << "ip = " << inet_ntoa(addr) << endl;

        addr.s_addr = inet_addr("10.211.55.3");//采用那个网卡作为多播的发送网卡
        //addr.s_addr = htonl(INADDR_ANY);//多个网卡不建议采用0.0.0.0这个地址，有可能出现接收端无法收到数据的情况
        len = sizeof(addr);
        setsockopt(server,IPPROTO_IP,IP_MULTICAST_IF,&addr,len);
        cout << "ip = " << inet_ntoa(addr) << endl;
    }

    sockaddr_in remote {};
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = inet_addr("224.1.1.168");
    remote.sin_port = htons(9000);
    len = sizeof(remote);

    char buf[32]{"WONGZEONJYU"};
    const int r(strlen(buf));

    while (true){

        sendto(server,buf,r,0,reinterpret_cast<const sockaddr * >(&remote),len);

        sleep(1);
    }

    close(server);

    return 0;
}
