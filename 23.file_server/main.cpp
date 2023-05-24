#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>
#include "message.h"
#include "msg_parser.h"
#include "tcp_client.h"
#include "tcp_server.h"
#include "udp_point.h"
#include "utility.h"

#define BUF_SIZE 512

using namespace std;
struct Handler{
    const char *cmd;
    void(*handler)(const char *);
};

enum {
    STOP,
    RUN,
    PAUSE,
};

static volatile const char *g_root {};
static volatile int g_status {};

static void DoResp(void * arg)
{
    cout << __FUNCTION__ << " : " << arg << endl;
}

static void *Client_Thread(void * arg)
{
    while (TcpClient_IsValid(arg)){
        DoResp(arg);
    }

    return arg;
}

static void * Server_Thread(void * arg)
{
    cout << __FUNCTION__ << " : " << arg << endl;

    while (TcpServer_IsValid(arg)){

        TcpClient * client {TcpServer_Accept(arg)};

        cout << __FUNCTION__ << " : " <<  client << endl;

        if (client && (PAUSE != g_status)){

            pthread_t tid {};
            pthread_create(&tid,nullptr,Client_Thread,client);

        }else {
            TcpClient_Del(client);
        }
    }

    return arg;
}

static void Start_Handler(const char *arg)
{
    int err {};

    if (STOP == g_status){

        TcpServer * server {TcpServer_New()};

        TcpServer_Start(server,9000,100);

        if (TcpServer_IsValid(server)){

            pthread_t tid {};
            err = pthread_create(&tid,nullptr,Server_Thread,server);
            cout << "err = " << err << endl;
        }
    }
}

static void Pause_Handler(const char *arg)
{

}

static void Exit_Handler(const char *arg)
{
    exit(0);
}

static Handler g_handler[]{
    {"start",Start_Handler},
    {"pause",Pause_Handler},
    {"exit",Exit_Handler},
};

static void Run(const char *root)
{
    std::cout << "File Server Demo @WongZeonJyu" << std::endl;

    g_root = root;

    for(;;){

        char line[BUF_SIZE]{};

        std::cout << "WongZeonJyu @ Input >>> ";

        cin >> line;

        //fgets(line,(sizeof(line) - 1),stdin);
        //line[strlen(line) - 1] = 0;
        //std::cout << "strlen(line) :" << strlen(line) << std::endl;
        // for (size_t i = 0; i < strlen(line); i++)
        // {
        //     std::cout << "line[" << i << "] : " << line[i] << std::endl;
        // }

        if (*line){
            
            char * cmd {FormatByChar(line,' ')};
            bool done{};

            for (size_t i {}; i < DIM(g_handler); i++){

                if (0 == strcmp(g_handler[i].cmd,cmd)){
                    g_handler[i].handler(g_handler[i].cmd);
                    done = true;
                    break;
                }
            }

            if (!done){
                std::cout << cmd << "can not be parsed!" << std::endl;
            }

            free(cmd);
        }
    }
}

int main(int argc, char const **argv)
{
    if (argc >= 2){
        
        DIR * dir {opendir(argv[1])};

        if(dir){
            
            closedir(dir);
            Run(argv[1]);
        }
    }

    std::cout << "opendir error" << std::endl;

    return 0;
}
