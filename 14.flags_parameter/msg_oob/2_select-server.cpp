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
    //int ret ( read(client,reinterpret_cast<void *>(buf),(sizeof(buf) - 1)) );
    auto ret { recv(client,buf,(sizeof(buf) - 1),0) };

    if (ret > 0){
        buf[ret] = 0;
        cout << "Receive :" << buf << '\n';
        ret = strcmp(buf,"quit") ? write(client,buf,ret) : -1 ;
    }

    return ret;
}

int main(int argc,char* argv[])
{
    const int server {socket(PF_INET,SOCK_STREAM,0)};

    if (-1 == server){
        cout << "server socket error\n";
        return -1;
    }

    sockaddr_in saddr {};

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);//htonl函数把小端转换成大端（网络字节序采用大端）
    saddr.sin_port = htons(8888);

    if ( -1 == bind( server,reinterpret_cast<const sockaddr *>(&saddr),sizeof(saddr) ) ){
        cout << "server bind error\n";
        return -1;
    }

    if ( -1 == listen(server,1) ){
        cout << "server listen error\n";
        return -1;
    }

    cout << "server start success\n" << 
            "server socket_fd :" << server << '\n';

    int max{server};
    fd_set reads{};

    FD_ZERO(&reads);
    FD_SET(server,&reads);
    
    for(;;) {

        fd_set temps { reads };
        fd_set except{ reads };
        timeval timeout{ .tv_sec = 0,.tv_usec = 10000 };

        const int num { select((max + 1),&temps,nullptr,&except,&timeout) };

        if (num > 0){

            for (int i {server}; i <= max; i++) {

                if (FD_ISSET(i,&except)){ /*通过判断异常去接收紧急数据*/
                    
                    if (i != server) {

                        char buf[2]{};
                        const auto r {recv(i,buf,sizeof(buf),MSG_OOB)}; /*紧急数据接收*/
                        if (r > 0){
                            buf[r] = 0;
                            cout << "OOB : "<< buf << '\n';
                        }
                    }
                }

                if (FD_ISSET(i,&temps)){

                    if (i == server) {
                        const int client {server_handler(server)};
                        if( client > -1 ){
                            FD_SET(client,&reads);
                            max = ((client > max) ? client : max);

                            cout << "accept client:" << client <<
                                    "\nmax:" << max << 
                                    "\nserver :" << server << '\n';
                        }
                    } else{
                        const int r { client_handler(i) };
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
