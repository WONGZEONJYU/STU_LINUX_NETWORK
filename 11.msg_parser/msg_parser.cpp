#include "msg_parser.h"
#include <cstring>
#include <malloc.h>
#include <netinet/in.h>
#include <unistd.h>
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

static Message * ToLastState(MsgParser * p)
{
    Message * ret {};

    if (p->header && (!p->header)){
        ret = p->msg;
        p->msg = nullptr;
    }

    return ret;
}

static void ntoh(Message * m)
{
    m->type = ntohs(m->type);
    m->cmd = ntohs(m->cmd);
    m->index = ntohs(m->index);
    m->total = ntohs(m->total);
    m->length = ntohl(m->length);
}

static int ToRecv(int fd,char * buf,int size)
{
    int retry{},i{};

    while (i < size){

        int len (read(fd,(buf +i),(size - i)));

        if (len > 0){
            i += len;
        }else if (len < 0){
            break;
        }else{  // 0 == len
            if (++retry > 5){
                break;
            }
            usleep(200 *1000);
        }
    }

    return i;
}

MParser * MParser_New()
{
    MParser * ret {calloc(1,sizeof(MsgParser))};

    if (ret){
        InitState(reinterpret_cast<MsgParser * >(ret));
    }

    return ret;
}

Message * MParser_ReadMem(MParser * parser,unsigned char * mem,unsigned int length)
{
    MsgParser * p { reinterpret_cast<MsgParser *>(parser) };

    Message * ret {};

    if (p && mem && length){
        
        if (!p->header){    //解析数据头
            
            if (p->need <= length){     //内存至少有12个字节才能解析数据头

                memcpy(&p->cache,mem,p->need);

                ntoh(&p->cache);//网络字节序转换成本机字节序

                mem += p->need;//内存偏移
                length -= p->need;//长度为内存剩余的字节数

                p->header = 1;//标记数据头解释完成
                p->need = p->cache.length;//所需的长度变为payload的长度

                ret = MParser_ReadMem(p,mem,length);//递归调用，进入读取payload环节
            }

        }else{

            if (!p->msg){   //半成品

                p->msg = reinterpret_cast<Message * >(malloc(sizeof(p->cache) + p->need));

                if (p->msg){
                    *p->msg = p->cache;
                }
            }

            if (p->msg){

                unsigned int len { (p->need < length) ? p->need : length };//取最小值

                unsigned int offset { p->msg->length - p->need };

                memcpy((p->msg->payload + offset),mem,len);

                p->need -= len;

            }

            if (!p->need){  //完整读取到全部所需数据，须进行解释器复位
                
                ret = p->msg;

                p->msg = nullptr;

                MParser_Reset(p);
            }
        }
    }

    return ret;
}

Message * MParser_Fd(MParser * parser,int fd) 
{
    Message * ret {};

    MsgParser * p {reinterpret_cast<MsgParser *>(parser)};

    if ((-1 != fd) && p){

        if (!p->header){    //解析数据头

            const int offset (sizeof(p->cache) - p->need);
            char * header {reinterpret_cast<char *>(&p->cache) + offset};
            const int len {ToRecv(fd,header,p->need)};

            if (len == p->need){

                ntoh(&p->cache);//网络字节序转换成本机字节序

                if (ToMidState(p)){     //切换到中间态
                    ret = MParser_Fd(p,fd); //递归调用,进入到读取payload分支
                }else{
                    InitState(p);
                }

            }else{  //还没读取够数据头所需的字节数
                p->need -= len; //计算数据头还需多少个字节
            }

        }else{
            
            if (p->msg){

                const int offset (p->msg->length - p->need);

                char * payload {reinterpret_cast<char *>(p->msg->payload)};

                const int len {ToRecv(fd,(payload + offset),p->need)};

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
    MsgParser * p {reinterpret_cast<MsgParser *>(parser)};

    if (p){
        InitState(p);
    }
}

void MParser_Del(MParser * parser)
{
    MsgParser * p {reinterpret_cast<MsgParser *>(parser)};

    if (p){
        free(p->msg);
        free(p);
    }
}
