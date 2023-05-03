#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "msg_parser.h"
#include "tcp_server.h"

using namespace std;

void EventListener(TcpClient * client,int evt){

    if (EVT_CONN == evt){

        cout << "Connect : " << client << endl;
        
    }else if (EVT_DATA == evt){
       
        Message * m { TcpClient_RecvMsg(client) };

        if (m) {

            // cout << "index = " << m->index << " total = " << m->total << " payload = " << m->payload << endl;

            // free(m);

            char * s { reinterpret_cast<char *>(TcpClient_GetData(client)) };

            //cout << "s = " << s << endl;
            //printf("s = %s\r\n",s);

            if (0 == m->index){

                s = reinterpret_cast<char *>(malloc(m->total + 1));

                TcpClient_SetData(client,s);
            }

            strcpy(s + m->index , reinterpret_cast<char *>(m->payload));

            if ((m->index + 1) == m->total){

                cout <<  "Data :" << s << endl;

                free(s);
            }

            free(m);
        }

    }else if (EVT_CLOSE == evt){
        cout << "Close :"  << client << endl;
    }
}

int main(int argc, char const *argv[])
{
    TcpServer * server {TcpServer_New()};

    if (server){

        int r { TcpServer_Start(server,8888,20) };

        cout << "r = " << r << endl;

        if (r)
        {
            TcpServer_SetListener(server,EventListener);

            TcpServer_DoWork(server);
        }

    }

    TcpServer_Del(server);

    return 0;
}
