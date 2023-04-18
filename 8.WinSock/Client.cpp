#include <iostream>
#include <WinSock2.h>

using namespace std;

int main()
{
    WSADATA wd{};

    if (WSAStartup(MAKEWORD(2,2) , &wd)){
        cout << "startup error" << endl;
        return -1;
    }

    SOCKET sock{ socket(PF_INET,SOCK_STREAM,IPPROTO_TCP) };

    if (INVALID_SOCKET == sock) {
        cout << "socket error" << endl;
        return -1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8888);

    if (SOCKET_ERROR == connect(sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr))) {
        cout << "connect error" << endl;
        return -1;
    }

    cout << "connect success sock :" << sock << endl;

    for (;;) {

        char input[32]{}, buf[128]{};

        cout << "Input: " << endl;

        cin >> input;

        int len(send(sock, input, strlen(input) + 1, 0));

        int r(recv(sock, buf, sizeof(buf), 0));

        if (r > 0) {
            cout << "Receive :" << buf << endl;
        }
        else {
            cout << "client quit r :" << r << endl;
            break;
        }
    }

    closesocket(sock);

    WSACleanup();

    return 0;
}

