#include <cstdio>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include "udp_point.h"

using namespace std;

int main(int argc, char const *argv[])
{
    UdpPoint * p { UdpPoint_New(8888) };

    std::cout << "p = " << p << std::endl;

    // sleep(5);

    // int nbytes { UdpPoint_Available(p) };

    // std::cout << "nbytes = " << nbytes << std::endl;

    char buf[128]{};
    char ip[16]{};
    int port{};

    Message * m { UdpPoint_RecvMsg(p,ip,&port)};

    //UdpPoint_RecvRaw(p,buf,sizeof(buf),ip,&port);

    cout << "ip : " << ip << endl;
    cout << "port : " << port << endl;
    //cout << "buf = " << hex << buf << endl;
    cout << "msg pointer: " << m << endl;

    cout << "msg->type = " << m->type << endl;
    cout << "msg->cmd = " << m->cmd << endl;
    cout << "msg->index = " << m->index << endl;
    cout << "msg->total = " << m->total << endl;

    for (size_t i {}; i < m->length; i++){
        cout << "0x" << hex << static_cast<uint16_t>(static_cast<uint8_t>(m->payload[i])) << " ";
    }

    // for (size_t i = 0; i < nbytes; i++) {
    //     cout << hex << static_cast<unsigned short>(static_cast<unsigned char>(buf[i]));
    // }

    cout << endl;

    //UdpPoint_SendRaw(p,ip,strlen(ip),ip,port);

    UdpPoint_SendMsg(p,m,ip,port);

    free(m);

    UdpPoint_Del(p);

    return 0;
}
