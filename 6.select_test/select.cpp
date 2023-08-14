#include <sys/select.h>
#include <sys/time.h>
#include <cstdio>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc,char* argv[])
{
    int iofd {};
    char s[] {"Hello World!\r\n"};
    fd_set reads{};
    
    FD_ZERO(&reads);
    FD_SET(iofd,&reads);

    int counter{};

    while (true){
        
        fd_set temps{reads};//select函数会影响temps的值，所以每次我们都需要拷贝一次以确保不会出错
        timeval timeout{.tv_sec = 0,.tv_usec = 50000};

        int r {select(1,&temps,nullptr,nullptr,&timeout)};

        if (r > 0){

            cout << "r :" << r << '\n';

            int len (read(iofd,s,(sizeof(s) - 1)));

            s[len] = 0;

            cout << "Input: " << s << '\n';

        }else if (0 == r){
            usleep(1000);
            if (++counter > 100){
                cout << "do something else\n";
                counter = 0;
            }
        }else{
            break;
        }
    }
    
    return 0;
}
