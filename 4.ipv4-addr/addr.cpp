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
    {
        in_addr addr0 {};
        if (inet_pton(AF_INET,"1.2.3.4",&addr0)){
            cout << "addr0 = 0x" << hex << addr0.s_addr << '\n';
        }
    }

    {
        char buf[32]{};
        in_addr addr1{0x0100007f};
        const char * r {inet_ntop(AF_INET,&addr1,buf,sizeof(buf))};
        cout << "addr1 = " << buf << '\n';
        cout << "r = " << r << '\n';
        cout << "buf addr: " << reinterpret_cast<int*>(buf) << '\n'; 
        cout << "r pointer to addr: " << reinterpret_cast<const int*>(r) << '\n';
    }

    return 0;
}
