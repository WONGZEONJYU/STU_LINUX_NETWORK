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
#include "page.h"
#include "response.h"

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

static void DoResp(TcpClient * client)
{
    int len {TcpClient_Available(client)};

    if (len > 0){

        char * buf {reinterpret_cast<char*>(malloc(len + 1))};

        TcpClient_RecvRaw(client,buf,len);

        buf[len] = 0;

        cout << __FUNCTION__  << " - Request buf : " << buf << endl;

        char resp[255] {};

        sscanf(buf,"GET %s HTTP",resp);

        std::cout << __FUNCTION__ <<" - Req String :" << resp << std::endl;

        const char *format {"HTTP/1.1 200 OK\r\n"
                        "Server:W.Z.J Http Server\r\n"
                        "Connect-Length:%d\r\n"
                        "Connect-Type:text/html\r\n"
                        "Connection:close\r\n"
                        "\r\n"
                        "%s"};

        sprintf(resp,format,strlen((const char *)g_root),g_root);
        
        TcpClient_SendRaw(client,resp,strlen(resp));

        TcpClient_Close(client);

        free(buf);
    }
}

static void *Process_Thread(void * arg)
{
    while (TcpClient_IsValid(arg)){
        DoResp(arg);
    }

    cout << __FUNCTION__  << "  -  Thread Exit : " << arg << endl;

    return arg;
}

static void * Server_Thread(void * arg)
{
    while (TcpServer_IsValid(arg)){

        TcpClient * client {TcpServer_Accept(arg)};

        if (client && (PAUSE != g_status)){

            pthread_t tid {};
            pthread_create(&tid,nullptr,Process_Thread,client);

        }else {
            TcpClient_Del(client);
        }
    }

    g_status = STOP;

    //cout << __FUNCTION__  << "  -  Thread Exit : " << arg << endl;

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

        }else {

            err = 1;
        }
    }

    if (!err){
        g_status = RUN;
        std::cout << "Server is OK!" << std::endl;
    }else {
        g_status = STOP;
        std::cout << "Server is failed!" << std::endl;
    }
}

static void Pause_Handler(const char *arg)
{
    if(RUN == g_status){

        g_status = PAUSE;

        std::cout << "Server is paused!" << std::endl;
    }else{
        std::cout << "Server is NOT started!" << std::endl;
    }
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
    // Table * t {CreatTable()};
    // RowInfo r1 {"aaa","bbbbb","cccc","dddd","eeeee"};
    // RowInfo r2 {"111","22222","3333","4444","55555"};

    // t = InsertRow(t,&r1);
    // t = InsertRow(t,&r2);

    // char *ts {ToTableString(t)};

    // char* page {ToPageString("test/path/folder",ts)};

    // std::cout << page << std::endl;

    // free(page);
    // free(ts);
    // FreeTable(t);
    RequestHandler(0,0,0);
    // getchar();
    
    return 0;

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


