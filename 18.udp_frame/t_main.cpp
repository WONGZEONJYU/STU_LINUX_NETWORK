#include <cstdio>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include "udp_point.h"

using namespace std;

static void udp_raw_test()
{
    auto p { UdpPoint_New(8888) };
    cout << "p = " << p << '\n';

    sleep(5);

    cout << "nbytes = " << UdpPoint_Available(p) << '\n';

    char buf[128]{},ip[16]{};
    int port{};
    UdpPoint_RecvRaw(p,buf,sizeof(buf),ip,&port);

    cout << "ip : " << ip << 
            "\nport : " << port << 
            "\nbuf = "  << buf << '\n';

    UdpPoint_SendRaw(p,ip,strlen(ip),ip,port);

    UdpPoint_Del(p);
}

static void message_test()
{
    auto p { UdpPoint_New(8888) };
    cout << "p = " << p << '\n';

    char ip[16]{};
    int port{};
    auto m { UdpPoint_RecvMsg(p,ip,&port) };

    cout << "ip : " << ip << 
            "\nport : " << port << 
            "\nmsg pointer: " << m <<
            "\nmsg->type = " << m->type << 
            "\nmsg->cmd = " << m->cmd << 
            "\nmsg->index = " << m->index << 
            "\nmsg->total = " << m->total << '\n';

    for (int i {}; i < m->length; i++) {
        cout << "0x" << hex << static_cast<int>(m->payload[i]) << " ";
    }

    cout << '\n';

    UdpPoint_SendMsg(p,m,ip,port);

    free(m);
    UdpPoint_Del(p);
}

int main(int argc, char const *argv[])
{
    //udp_raw_test();
    //message_test();
    return 0;
}
