#include <cstdio>
#include <unistd.h>
#include <iostream>

using namespace std;

int main()
{
    int iofd {0};

    char s[] {"Hello World!\r\n"};

    write(iofd,s,sizeof(s));

    int len (read(iofd,s,5));

    s[len] = 0;

    std::cout << s << std::endl;

    return 0;
}
