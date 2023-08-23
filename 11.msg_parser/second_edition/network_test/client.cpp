#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "../../message.h"

using namespace std;

int main(int argc,char* argv[]) 
{
    int sock{socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == sock){
        cout << "socket error\n";
        return -1;
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8888);

    if ( -1 == connect( sock,reinterpret_cast<sockaddr *>(&addr),sizeof(addr) )){
        cout << "connect error\n";
        return -1;
    }

    cout << "connect success sock :" << sock << '\n';

    constexpr char test[] {"HelloWorld"};

    for (int i {}; i < strlen(test); i++){

        char buf[2]{};/*存一个字符的字符串,数组为2是还有一个字符串结束符 `\0` */

        buf[0] = test[i];

        Message *pm { Message_New(128,129,i,strlen(test),buf,sizeof(buf)) };

        Message_H2N(pm);

        send(sock,pm,(sizeof(Message) + sizeof(buf)),0);

        //usleep(1000);

        free(pm);
    }

    getchar();

    close(sock);

    return 0;
}
