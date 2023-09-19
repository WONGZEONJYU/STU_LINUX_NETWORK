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
    const int sock{socket(PF_INET,SOCK_DGRAM,0)};

    if (-1 == sock){
        cout << "socket error\n";
        return -1;
    }

    cout << "create socket success :" << sock << '\n';

    sockaddr_in local {};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(9000);

    if ( -1 == ::bind( sock,reinterpret_cast<const sockaddr *>(&local),sizeof(local) ) ){
        cout << "udp bind error\n";
        return -1;
    }

    ip_mreq group{};
    group.imr_multiaddr.s_addr = inet_addr("224.1.1.168");	//发送端发往多播数据的IP地址
    group.imr_interface.s_addr = htonl(INADDR_ANY); /*单网卡没有问题,多网卡建议手动确定那个网卡*/

    setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&group,sizeof(group));		//接收端加入多播组设置

    for(;;) {

        char input[32]{},buf[128]{};

        sockaddr_in remote {};
        socklen_t len {sizeof(remote)};

        const auto r {recvfrom(sock,buf,sizeof(buf),0,reinterpret_cast<sockaddr * >(&remote),&len)};

        if (r > 0){

            buf[r] = 0;
            cout << "ip = " << inet_ntoa(remote.sin_addr) <<
                    "\nport = " << ntohs(remote.sin_port) << 
                    "\nReceive : " << buf << '\n';
        }else{
            break;
        }
    }

    close(sock);
    return 0;
}
