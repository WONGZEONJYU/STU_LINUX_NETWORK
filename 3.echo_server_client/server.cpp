#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <type_traits>
#include <functional>

using namespace std;

static function<void(int,siginfo_t*,void*)> signal_func;

void signal_handler(const int sig,siginfo_t *info, void*)
{
    signal_func(sig,info,nullptr);
}

int main(int argc,char* argv[])
{
    int server {socket(PF_INET,SOCK_STREAM,0)};
    int client {-1};
    
    if (-1 == server){
        cout << "server socket error\n";
        return -1;
    }

    {
        /*此处跟网络无关,仅仅是为了ctrl+c终止程序销毁server,client*/
        struct sigaction act{};
        act.sa_flags = SA_RESTART | SA_SIGINFO ;
        act.sa_sigaction = signal_handler;

        signal_func = move([=](const int sig,siginfo_t* info,void*)mutable{
            constexpr char str[] {"\nexit\n"};
            write(1,str,sizeof(str));
            close(client);
            close(server);
            exit(0);
        });

        sigaction(SIGINT,&act,nullptr);
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

    cout << "server start success\n";

    for(;;){

        sockaddr_in caddr {};
        socklen_t asize {sizeof(caddr)};

        client = accept(server,reinterpret_cast<sockaddr *>(&caddr),&asize);

        if (-1 == client){
            cout << "client accept error\n";
            return -1;
        }

        cout << "client :" << client << '\n'; //client的数值表示系统资源的id

        int r {},len{};

        do{
            char buf[32]{};

            r = recv(client,buf,(sizeof(buf)/sizeof(*buf)),0);

            if (r > 0){

                cout << "Server Receive :" << buf << '\n';

                if ( strcmp(buf,"quit") ){

                    len = send(client,buf,r,0);

                }else{ /*0 == strcmp(...) 跳出do while , 客户端断开连接*/
                   break; 
                }
            }

        } while (r > 0);

        close(client);
    }

    close(server);

    return 0;
}

