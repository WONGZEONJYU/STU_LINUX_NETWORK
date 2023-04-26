#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

int server_handler(int server)
{
    sockaddr_in addr{};
    socklen_t asize {sizeof(addr)};
    return accept(server,reinterpret_cast<sockaddr *>(&addr),&asize);
}

int client_handler(int client)
{
    char buf[32]{};

    int ret ( read(client,reinterpret_cast<void *>(buf),(sizeof(buf) - 1)) );

    if (ret > 0){

        buf[ret] = 0;

        std::cout << "Receive :" << buf << std::endl;

        if (strcmp(buf,"quit")){
            
            ret = write(client,reinterpret_cast<const void *>(buf),ret);
        }else{
            ret = -1;
        }
    }

    return ret;
}

int main() 
{
    const int server {socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == server){
        cout << "server socket error" << endl;
        return -1;
    }

    sockaddr_in saddr {};

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);//htonl函数把小端转换成大端（网络字节序采用大端）
    saddr.sin_port = htons(8888);

    if ( -1 == bind( server,reinterpret_cast<const sockaddr *>(&saddr),sizeof(saddr) ) ){
        cout << "server bind error" << endl;
        return -1;
    }

    if ( -1 == listen(server,1) ){
        cout << "server listen error" << endl;
        return -1;
    }

    cout << "server start success" << endl;

    cout << "server socket_fd :" << server << endl;

    int max{server};
    fd_set reads{};

    FD_ZERO(&reads);
    FD_SET(server,&reads);

    for(;;) {

        fd_set temps { reads };

        timeval timeout{ .tv_sec = 0,.tv_usec = 10000 };

        int num{ select((max + 1),&temps,nullptr,nullptr,&timeout) };

        if (num > 0){

            for (int i {server}; i <= max; i++) {

                if (FD_ISSET(i,&temps)){

                    if (i == server) {

                        int client {server_handler(server)};

                        if( client > -1 ){

                            FD_SET(client,&reads);

                            max = ((client > max) ? client : max);

                            std::cout << "accept client:" << client << std::endl;
                            std::cout << "max:" << max << std::endl;
                            std::cout << "server :" << server << std::endl;
                        }

                    } else{
                        
                        int r { client_handler(i) };

                        if (-1 == r){
                            
                            FD_CLR(i,&reads);

                            close(i);
                        }
                    }
                }
            }
        }
    }

    close(server);

    return 0;
}
