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
    unsigned int addr = inet_addr("1.2.3.4");
    in_addr addr1 {0x09080706};
    in_addr addr2 {0x05040302};

    char * s1 {strcpy(new char[32],inet_ntoa(addr1))};
    char * s2 {strcpy(new char[32],inet_ntoa(addr2))};

    std::cout << "addr1 = " << hex << addr1.s_addr << std::endl;
    std::cout << "addr2 = " << hex << addr2.s_addr << std::endl;

    std::cout << "s1 = " << s1 << std::endl;
    std::cout << "s2 = " << s2 << std::endl;

    std::cout << "addr = " << hex << addr  << std::endl;

    if (inet_aton("1.2.3.4",&addr1)){
        std::cout << "addr1 = " << std::hex << addr1.s_addr << std::endl;
    }
    
    delete s1;
    delete s2;

    return 0;
}
