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
    // sleep(5);
    // int nbytes { UdpPoint_Available(p) };
    // cout << "nbytes = " << nbytes << '\n;
}

int main(int argc, char const *argv[])
{

    udp_raw_test();
    
    char buf[128]{},ip[16]{};
    int port{};

    auto m { UdpPoint_RecvMsg(p,ip,&port) };

    //UdpPoint_RecvRaw(p,buf,sizeof(buf),ip,&port);

    cout << "ip : " << ip << 
            "\nport : " << port << 
            //"\nbuf = " << hex << buf <<
            "\nmsg pointer: " << m <<
            "\nmsg->type = " << m->type << 
            "\nmsg->cmd = " << m->cmd << 
            "\nmsg->index = " << m->index << 
            "\nmsg->total = " << m->total << '\n';

    for (int i {}; i < m->length; i++) {
        cout << "0x" << hex << static_cast<int>(m->payload[i]) << " ";
    }

    // for (int i {}; i < nbytes; i++) {
    //     cout << hex << static_cast<unsigned short>(static_cast<unsigned char>(buf[i]));
    // }

    cout << '\n';

    //UdpPoint_SendRaw(p,ip,strlen(ip),ip,port);

    UdpPoint_SendMsg(p,m,ip,port);
    free(m);
    UdpPoint_Del(p);
    return 0;
}
