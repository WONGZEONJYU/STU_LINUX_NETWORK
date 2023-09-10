#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "msg_parser.h"
#include "tcp_server.h"

using namespace std;

void EventListener(TcpClient* client,int evt){

    if (EVT_CONN == evt){

        cout << "Connect : " << client << '\n';
        
    }else if (EVT_DATA == evt){
       
        auto m { TcpClient_RecvMsg(client) };

        if (m) {

            // cout << "index = " << m->index << " ,total = " << m->total << " ,payload = " << m->payload << '\n';
            // free(m);

            auto s { reinterpret_cast<char *>(TcpClient_GetData(client)) };

            if (0 == m->index){ /*一系列协议消息中的第一个消息*/

                s = reinterpret_cast<char *>(malloc(m->total + 1));
                TcpClient_SetData(client,s);
            }

            /*m->index在每一条协议上不一样,假如有10条消息,这10条消息的index的值是[0~9],每一条消息上的total都为10*/
            /*由于客户端是把消息分开发送,所以这里进行了组合*/
            strcpy(s + m->index , reinterpret_cast<const char *>(m->payload));

            if ((m->index + 1) == m->total){ /*一系列消息收完了*/
                cout << "Data :" << s << '\n';
                free(s);
            }

            free(m);
        }

    }else if (EVT_CLOSE == evt){
        cout << "Close :"  << client << '\n';
    }
}

int main(int argc, char const *argv[])
{
    auto server { TcpServer_New() };

    if (!TcpServer_IsValid(server)){

        int r { TcpServer_Start(server,8888,20) };

        cout << "r = " << r << '\n';

        if (r)
        {
            TcpServer_SetListener(server,EventListener);

            TcpServer_DoWork(server);
        }
           TcpServer_Del(server);
    }

#if 0
    TcpServer_Start(server,9000,100);

    if (TcpServer_IsValid(server)){

        auto c {TcpServer_Accept(server)};

        if (c){

            char buf[64]{};

            const auto len {TcpClient_RecvRaw(c,buf,sizeof(buf) - 1)};

            buf[len] = 0;

            cout << "recv = " << buf << '\n';

            TcpClient_SendRaw(c,buf,len);

            TcpClient_Del(c);
        }

        TcpServer_Del(server);
    }
#endif

    return 0;
}
