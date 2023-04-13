#include <sys/select.h>
#include <sys/time.h>
#include <cstdio>
#include <unistd.h>
#include <iostream>

using namespace std;

int main()
{
    int iofd {};
    char s[] {"Hello World!\r\n"};
    fd_set reads{};
    fd_set temps{};
    timeval timeout{};
    FD_ZERO(&reads);
    FD_SET(iofd,&reads);

    int counter{};

    while (true){
        
        int r {-1};
        
        temps = reads;//select函数会影响temps的值，所以每次我们都需要拷贝一次以确保不会出错

        timeout.tv_sec = 0;
        timeout.tv_usec = 50000;

        r = select(1,&temps,nullptr,nullptr,&timeout);

        if (r > 0){
            int len (read(iofd,s,sizeof(s) -1));

            s[len] = 0;

            std::cout << "Input: " << s << std::endl;

        }else if (0 == r){
            usleep(10000);

            if (++counter > 100){
                std::cout << "do something else" << std::endl;
                counter = 0;
            }
        }else{
            break;
        }
    }
    
    return 0;
}
