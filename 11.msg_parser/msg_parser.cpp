#include "msg_parser.h"
#include <cstring>
#include <malloc.h>
#include <netinet/in.h>

struct MsgParser
{
    int header;         //标识消息头是否解析成功
    int need;           //标识还需要多少字节才能完成解析    (初始化状态是标识需多少个字节才能解析出固定部分)
    Message * msg;      //解析中的协议消息(半成品)
    Message cache;      //缓存已解析的消息头
};

MParser * MParser_New()
{
    MParser * ret {reinterpret_cast<MParser * >(calloc(1,sizeof(MsgParser)))};

    MParser_Reset(ret);

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

                p->cache.type = ntohs(p->cache.type);
                p->cache.cmd = ntohs(p->cache.cmd);
                p->cache.index = ntohs(p->cache.index);
                p->cache.total = ntohs(p->cache.total);
                p->cache.length = ntohl(p->cache.length);

                mem += p->need;//内存偏移
                length -= p->need;//长度为内存剩余的字节数

                p->header = 1;//标记数据头解释完成
                p->need = p->cache.length;//所需的长度变为payload的长度

                ret = MParser_ReadMem(p,mem,length);//递归调用，会进入else分支
            }

        }else{

            if (!p->msg){

                p->msg = reinterpret_cast<Message * >(malloc(sizeof(p->cache) + p->need));

                if (p->msg){
                    *p->msg = p->cache;
                }
            }

            if (p->msg){

                unsigned int len { (p->need < length) ? p->need : length };

                unsigned int offset { p->msg->length - p->need };

                memcpy((p->msg->payload + offset),mem,len);

                p->need -= len;

            }

            if (!p->need){
                
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

    return ret;
}

void MParser_Reset(MParser * parser)
{
    MsgParser * p {reinterpret_cast<MsgParser *>(parser)};

    if (p){

        p->header = 0;

        p->need = sizeof(p->cache);

        if (p->msg){
            free(p->msg);
        }

        p->msg = nullptr;
    }
}

void MParser_Del(MParser * parser)
{
    MsgParser * p {reinterpret_cast<MsgParser *>(parser)};

    if (p){

        if (p->msg){
            free(p->msg);
            free(p);
        }
    }
}
