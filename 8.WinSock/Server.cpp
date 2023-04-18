#include <iostream>
#include <WinSock2.h>
#include <vector>

using namespace std;

int main()
{
    WSADATA wd{};

    if (WSAStartup(MAKEWORD(2,2),&wd)){
        cout << "startup error" << endl;
        return -1;
    }

    SOCKET server{ socket(PF_INET,SOCK_STREAM,IPPROTO_TCP) };

    if (INVALID_SOCKET == server) {
        cout << "server socket error" << endl;
        return -1;
    }

    sockaddr_in saddr{};

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);//htonl函数把小端转换成大端（网络字节序采用大端）
    saddr.sin_port = htons(8888);

    if (SOCKET_ERROR == bind(server, reinterpret_cast<const sockaddr*>(&saddr), sizeof(saddr))) {
        cout << "server bind error" << endl;
        return -1;
    }

    if (SOCKET_ERROR == listen(server, 1)) {
        cout << "server listen error" << endl;
        return -1;
    }

    cout << "server start success" << endl;

    while (true) {

        sockaddr_in caddr{};
        int asize{ sizeof(caddr) };

        SOCKET client{ accept(server,reinterpret_cast<sockaddr*>(&caddr),&asize) };

        if (INVALID_SOCKET == client) {
            cout << "client accept success" << endl;
            return -1;
        }

        cout << "client :" << client << endl; //client的数值表示系统资源的id

        int r{}, len{};

        do {
            char buf[32]{};

            r = recv(client, buf, (sizeof(buf) / sizeof(char)), 0);

            if (r > 0) {

                cout << "Server Receive :" << buf << endl;

                if (strcmp(buf, "quit")) {

                    len = send(client, buf, r, 0);
                }
                else {
                    break;
                }
            }

        } while (r > 0);

        closesocket(client);
    }

    closesocket(server);

    WSACleanup();

    return 0;
}

