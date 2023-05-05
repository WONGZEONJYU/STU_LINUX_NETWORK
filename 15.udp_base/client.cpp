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
    int sock{socket(PF_INET,SOCK_DGRAM,0)};

    if (-1 == sock){
        cout << "socket error" << endl;
        return -1;
    }

    cout << "create socket success :" << sock << endl;

    sockaddr_in local {};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(9999);

    if ( -1 == bind( sock,reinterpret_cast<const sockaddr *>(&local),sizeof(local) ) ){
        cout << "udp bind error" << endl;
        return -1;
    }

    while(true) {

        char input[32]{},buf[128]{};

        sockaddr_in remote {};
        remote.sin_family = AF_INET;
        remote.sin_addr.s_addr = inet_addr("127.0.0.1");
        remote.sin_port = htons(8888);

        cout << "Input: " << endl;

        cin >> input;

        socklen_t len {sizeof(remote)};

        sendto(sock,input,strlen(input),0,reinterpret_cast<const sockaddr * >(&remote),len);

        int r (recvfrom(sock,buf,sizeof(buf),0,reinterpret_cast<sockaddr * >(&remote),&len));

        if (r > 0){

            buf[r] = 0;

            cout << "Receive :" << buf << endl;

            if(0 == strcmp(buf,"quit")){
                break;
            }

        }else{
            break;
        }
    }

    close(sock);

    return 0;
}
