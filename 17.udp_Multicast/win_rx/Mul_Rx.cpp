#include <iostream>
#include <WinSock2.h>
#include <ws2ipdef.h>

//需在工程设置链接器(linker)输入选项添加ws2_32.lib

using namespace std;

int main(int argc,char* argv[])
{
    WSADATA wd{};

    if (WSAStartup(MAKEWORD(2, 2), &wd)) {
        cout << "startup error\n";
        return -1;
    }

    SOCKET sock{ socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP) };

    if (INVALID_SOCKET == sock) {
        cout << "socket error\n";
        return -1;
    }

    cout << "connect success sock :" << sock << '\n';

    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    //local.sin_addr.s_addr = inet_addr("192.168.199.1");
    local.sin_port = htons(9000);

    if (SOCKET_ERROR == bind(sock, reinterpret_cast<const sockaddr *>(&local), sizeof(local))) {
        cout << "udp bind error\n";
        return -1;
    }

    ip_mreq group{};
    group.imr_multiaddr.s_addr = inet_addr("224.1.1.168");			//发送端发往多播数据的IP地址

    /*加入组播的地址,存在多个网卡时不建议使用INADDR_ANY*/
    //group.imr_interface.s_addr = htonl(INADDR_ANY);
    group.imr_interface.s_addr = inet_addr("10.10.10.152");        

    setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
        reinterpret_cast<const char *>(&group), sizeof(group));		//接收端加入多播组设置

    for (;;) {
        char input[32]{}, buf[128]{};
        sockaddr_in remote{};
        int len{ sizeof(remote) };
        const auto r{ recvfrom(sock, buf, sizeof(buf), 0, reinterpret_cast<sockaddr*>(&remote), &len) };

        if (r > 0) {
            cout << "ip = " << inet_ntoa(remote.sin_addr) <<
                "\nport = " << ntohs(remote.sin_port) <<
                "\nReceive : " << buf << '\n';
        }else {
            break;
        }
    }
 
    closesocket(sock);

    WSACleanup();

    return 0;
}

