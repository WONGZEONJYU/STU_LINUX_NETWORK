#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include "udp_point.h"

using namespace std;

int main(int argc, char const *argv[])
{
    UdpPoint * p{ UdpPoint_New(8888) };

    std::cout << "p = " << p << std::endl;

    int i { UdpPoint_Available(p) };

    std::cout << "i = " << i << std::endl;

    UdpPoint_Del(p);

    return 0;
}
