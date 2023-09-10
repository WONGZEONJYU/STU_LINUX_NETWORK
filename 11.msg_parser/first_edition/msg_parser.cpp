#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "msg_parser.h"

using namespace std;

struct MsgParser
{
    int header;      // 标识消息头是否解析成功
    int need;        // 标识还需要多少字节才能完成解析
    Message* msg;    // 解析中的协议消息（半成品）
    Message cache;   // 缓存已解析的消息头
};

MParser* MParser_New()
{
    auto ret {calloc(1,sizeof(MsgParser))};
    MParser_Reset(ret);
    return ret;
}

Message* MParser_ReadMem(MParser* parser, unsigned char* mem, unsigned int length)
{
    auto p {reinterpret_cast<MsgParser*>(parser)};

    Message* ret {};

    if (p && mem && length){

        if (!p->header){

            if (p->need <= length){

                memcpy(&p->cache,mem,p->need);

                p->cache.type = ntohs(p->cache.type);
                p->cache.cmd = ntohs(p->cache.cmd);
                p->cache.index = ntohs(p->cache.index);
                p->cache.total = ntohs(p->cache.total);
                p->cache.length = ntohl(p->cache.length);

                // cout << "p->cache.length = " << p->cache.length << '\n';
                // cout << "length = " << length << '\n';

                mem += p->need;
                length -= p->need;

                // cout << "length = " << length << '\n';

                p->header = 1;
                p->need = p->cache.length;
                
                
                ret = MParser_ReadMem(p,mem,length);
            }
            
        }else{
            
            if (!p->msg){

                p->msg = reinterpret_cast<Message*>(malloc(sizeof(p->cache) + p->need));

                if (p->msg){
                    *p->msg = p->cache;
                }
            }

            if (p->msg){
                
                // cout << "p->need = " << p->need << '\n';
                // cout << "length = " << length << '\n';
                // cout << "p->msg->length = " << p->msg->length << '\n';

                // unsigned int len { (p->need < length )? p->need : length};
                // unsigned int offset { p->msg->length - p->need };

                auto len { (p->need < length )? p->need : length};
                auto offset { p->msg->length - p->need };

                // cout << "len = " << len << '\n';
                // cout << "offset = " << offset << '\n';

                memcpy(p->msg->payload + offset,mem,len);
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

Message* MParser_ReadFd(MParser* parser, int fd)
{
    auto p {reinterpret_cast<MsgParser*>(parser)};
    Message* ret {};

    return ret;
}

void MParser_Reset(MParser* parser)
{
    auto p {reinterpret_cast<MsgParser*>(parser)};

    if (p){
        p->header = 0;
        p->need = sizeof(p->cache);

        if (p->msg){
            free(p->msg);
        }
        p->msg = nullptr;
    }
}

void MParser_Del(MParser* parser)
{
    auto p {reinterpret_cast<MsgParser*>(parser)};
    if( p ){
        if( p->msg ){
            free(p->msg);
            free(p);
        }
    }
}
