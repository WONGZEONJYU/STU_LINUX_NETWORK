#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include "msg_parser.h"

struct MsgParser
{
    int header;         //标识消息头是否解析成功
    int need;           //标识还需要多少字节才能完成解析    (初始化状态是标识需多少个字节才能解析出固定部分)
    Message * msg;      //解析中的协议消息(半成品)
    Message cache;      //缓存已解析的消息头
};

static void InitState(MsgParser * p)
{
    p->header = 0;
    p->need = sizeof(p->cache);
    free(p->msg);
    p->msg = nullptr;
}

static int ToMidState(MsgParser * p)
{
    p->header = 1;
    p->need = p->cache.length;
    p->msg = reinterpret_cast<Message *>(malloc(sizeof(p->cache) + p->need));
    if (p->msg){
        *p->msg = p->cache;
    }
    return !!p->msg;
}

static Message *ToLastState(MsgParser * p)
{
    Message * ret {};

    if (p->header && (!p->need)){
        ret = p->msg;
        p->msg = nullptr;
    }

    return ret;
}

static int ToRecv(const int fd,char * buf,const int size)
{
    int retry{},i{};

    while (i < size){

        auto len {read(fd,(buf + i),(size - i))};

        if (len > 0){
            i += len;
        }else if (len < 0){ //读取数据出错 -1 == len
            break;
        }else{   //0 == len
            if (++retry > 5){
                break;
            }
            usleep(200 *1000);
        }
    }

    return i;
}

MParser* MParser_New()
{
    auto ret {calloc(1,sizeof(MsgParser))};

    if (ret){
        InitState(static_cast<MsgParser * >(ret));
    }

    return ret;
}

Message* MParser_ReadMem(MParser* parser,unsigned char * mem,unsigned int length)
{
    auto p { static_cast<MsgParser *>(parser) };

    Message * ret {};

    if (p && mem){

        if (!p->header){    //解析数据头

            const auto len {(p->need < length) ? p->need : length};  //取最小值，内存中的数据长度不一定达到协议数据头所需的字节数
            const auto offset {sizeof(p->cache) - p->need };          //计算消息头存储区的偏移量
            auto _cache { reinterpret_cast<char *>(&p->cache) + offset };

            memcpy(_cache,mem,len);

            if (len == p->need){     //内存至少有12个字节才能解析数据头

                Message_N2H(&p->cache);//网络字节序转换成本机字节序

                mem += p->need;     //内存偏移
                length -= p->need;  //内存长度减去已经读取的header的个数

                if (ToMidState(p)){
                    ret = MParser_ReadMem(p,mem,length);//递归调用，进入读取payload环节
                }else{
                    InitState(p);
                }

            }else{
                p->need -= len;
            }

        }else{

            if (p->msg){

                const auto len {(p->need < length) ? p->need : length};
                const auto offset {p->msg->length - p->need};   /*计算偏移量*/
                memcpy((p->msg->payload + offset),mem,len);
                p->need -= len;
            }

            if (ret = ToLastState(p)){
                InitState(p);
            }
        }
    }

    return ret;
}

Message* MParser_ReadFd(MParser * parser,int fd) 
{
    Message * ret {};

    auto p {static_cast<MsgParser *>(parser)};

    if ((-1 != fd) && p){

        if (!p->header){    //解析数据头

            const auto offset {sizeof(p->cache) - p->need};
            auto header {reinterpret_cast<char *>(&p->cache) + offset};
            const auto len {ToRecv(fd,header,p->need)};

            if (len == p->need){

                Message_N2H(&p->cache);//网络字节序转换成本机字节序

                if (ToMidState(p)){     //切换到中间态
                    ret = MParser_ReadFd(p,fd); //递归调用,进入到读取payload分支
                }else{
                    InitState(p);
                }

            }else{  //还没读取够数据头所需的字节数
                p->need -= len; //计算数据头还需多少个字节
            }

        }else{

            if (p->msg){

                const auto offset {p->msg->length - p->need};

                auto payload {reinterpret_cast<char *>(p->msg->payload)};

                const auto len {ToRecv(fd,(payload + offset),p->need)};

                p->need -= len;
            }

            if (ret = ToLastState(p)){
                InitState(p);
            }
        }
    }

    return ret;
}

void MParser_Reset(MParser * parser)
{
    auto p {static_cast<MsgParser *>(parser)};

    if (p){
        InitState(p);
    }
}

void MParser_Del(MParser * parser)
{
    auto p {static_cast<MsgParser *>(parser)};

    if (p){
        free(p->msg);
        free(p);
    }
}
