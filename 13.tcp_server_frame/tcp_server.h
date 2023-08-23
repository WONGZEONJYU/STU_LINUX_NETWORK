#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include "tcp_client.h"

using TcpServer = void;
using Listener = void(*)(TcpClient* ,int);

//第一个参数是用于通信的Client(该类型在客户端通信框架有说明)
//第二个参数是事件值,用于标识事件类型

enum    //事件类型定义
{
    EVT_CONN,    //连接事件
    EVT_DATA,    //数据事件
    EVT_CLOSE,    //断连事件
};

TcpServer* TcpServer_New();
int TcpServer_Start(TcpServer* server,int port,int max);    //开始监听
void TcpServer_Stop(TcpServer* server);    //停止监听
void TcpServer_SetListener(TcpServer* server,Listener listener);    //设置回调函数
int TcpServer_IsValid(TcpServer* server);    //检查服务端是否有效
void TcpServer_DoWork(TcpServer* server);    //用于服务端工作
TcpClient* TcpServer_Accept (TcpServer* server);
void TcpServer_Del(TcpServer* server);    //销毁服务端
int TcpServer_SetOpt(TcpServer* server,int level, int optname, 
                    const void* optval, unsigned int optlen);
int TcpServer_GetOpt(TcpServer* server, int level, int optname, 
                    void* optval, unsigned int* optlen);

#endif
